defmodule Pond do
  @moduledoc "The single pond. PostgreSQL owns identity, ordering and idempotency."
  alias Pond.Repo
  def now, do: System.system_time(:second)
  defp query(sql, params \\ []), do: Repo.query!(sql, params).rows

  def identity(credential) when is_binary(credential) do
    if Regex.match?(~r/\A[0-9a-f]{64}\z/, credential) do
      hash = :crypto.hash(:sha256, credential)
      <<id::binary-size(16), _::binary>> = hash
      {:ok, id, hash}
    else
      {:error, :unauthorized}
    end
  end

  def identity(_), do: {:error, :unauthorized}

  def register(credential, at \\ now()) do
    with {:ok, id, hash} <- identity(credential) do
      Repo.transaction(fn ->
        query("SELECT pg_advisory_xact_lock(7061)")
        [[count]] = query("SELECT count(*) FROM installations")

        if count >= Application.get_env(:pond, :installation_limit, 10000) and
             query("SELECT id FROM installations WHERE id=$1", [id]) == [],
           do: Repo.rollback(:capacity)

        # Serialises bootstrap retry with deletion, including before a row exists.
        query("SELECT pg_advisory_xact_lock(hashtextextended($1, 0))", [Base.encode16(id)])
        if query("SELECT id FROM deletions WHERE id=$1", [id]) != [], do: Repo.rollback(:revoked)

        query(
          "INSERT INTO installations(id, credential_hash, last_seen) VALUES ($1,$2,$3) ON CONFLICT(id) DO NOTHING",
          [id, hash, at]
        )

        %{id: Ecto.UUID.load!(id), version: 1}
      end)
    end
  end

  def authenticate(credential, at \\ now()) do
    with {:ok, id, hash} <- identity(credential),
         [[^id]] <-
           query(
             "UPDATE installations SET last_seen=$3 WHERE id=$1 AND credential_hash=$2 AND last_seen>$3-2592000 RETURNING id",
             [id, hash, at]
           ) do
      {:ok, id}
    else
      _ -> {:error, :unauthorized}
    end
  end

  def drop(id, request_id, source, at \\ nil) do
    with {:ok, request} <- Ecto.UUID.dump(request_id),
         true <-
           is_nil(source) or (is_integer(source) and source >= 0 and source <= 2_147_483_647) do
      Repo.transaction(fn ->
        query("SELECT pg_advisory_xact_lock_shared(7060)")

        if query("SELECT id FROM installations WHERE id=$1 FOR UPDATE", [id]) == [],
          do: Repo.rollback(:unauthorized)

        at = at || now()

        case query(
               "SELECT accepted_at, window_id, source_bundle FROM drops WHERE installation_id=$1 AND request_id=$2",
               [id, request]
             ) do
          [[accepted, window_id, echo]] when accepted > at - 86400 ->
            receipt(request_id, accepted, window_id, echo, true)

          [_] ->
            Repo.rollback(:expired_request)

          [] ->
            [[count, last]] =
              query(
                "SELECT count(*), max(accepted_at) FROM drops WHERE installation_id=$1 AND accepted_at>$2-86400",
                [id, at]
              )

            if count >= 100 or (last && at - last < 10), do: Repo.rollback(:rate_limited)
            echo = valid_source(id, source, at)
            window_id = div(at, 60)

            query(
              "INSERT INTO drops(installation_id,request_id,accepted_at,window_id,source_bundle) VALUES($1,$2,$3,$4,$5)",
              [id, request, at, window_id, echo]
            )

            receipt(request_id, at, window_id, echo, false)
        end
      end)
    else
      _ -> {:error, :invalid_request}
    end
  end

  defp valid_source(_id, nil, _at), do: nil

  defp valid_source(id, source, at) do
    case query(
           """
           SELECT b.id FROM bundles b JOIN issued_bundles i ON i.bundle_id=b.id
           WHERE b.id=$1 AND i.installation_id=$2 AND (b.id+1)*60+120>$3
             AND (b.id+1)*60<=$3 AND i.issued_at>$3-86400
             AND EXISTS(SELECT 1 FROM drops d WHERE d.window_id=b.id AND d.installation_id<>$2 AND d.accepted_at>$3-86400)
           """,
           [source, id, at]
         ) do
      [[^source]] -> source
      _ -> nil
    end
  end

  defp receipt(request, at, window_id, echo, duplicate),
    do: %{
      version: 1,
      request_id: request,
      accepted_at: at,
      bundle_id: window_id,
      kind: if(echo, do: "echo", else: "drop"),
      duplicate: duplicate
    }

  def finalize(at \\ now()) do
    Repo.transaction(fn ->
      query("SELECT pg_advisory_xact_lock(7060)")

      query(
        """
        INSERT INTO bundles(id,drops,echoes)
        SELECT window_id,count(*)::integer,count(source_bundle)::integer FROM drops
        WHERE window_id<$1 AND accepted_at>$2-86400 GROUP BY window_id
        ON CONFLICT(id) DO NOTHING
        """,
        [div(at, 60), at]
      )
    end)
  end

  def pond(id, cursor, at \\ now()) do
    with {:ok, after_window} <- decode_cursor(id, cursor), {:ok, _} <- finalize(at) do
      Repo.transaction(fn ->
        if query("SELECT id FROM installations WHERE id=$1 FOR UPDATE", [id]) == [],
          do: Repo.rollback(:unauthorized)

        latest =
          query(
            "SELECT id,drops,echoes FROM bundles WHERE id>$1 AND (id+1)*60+120>$2 AND (id+1)*60<=$2 ORDER BY id DESC LIMIT 1",
            [after_window, at]
          )

        bundle =
          case latest do
            [[window_id, drops, echoes]] ->
              [[other]] =
                query(
                  "SELECT EXISTS(SELECT 1 FROM drops WHERE window_id=$1 AND installation_id<>$2 AND accepted_at>$3-86400)",
                  [window_id, id, at]
                )

              [[answer]] =
                query(
                  "SELECT EXISTS(SELECT 1 FROM drops e JOIN drops mine ON mine.window_id=e.source_bundle WHERE e.window_id=$1 AND mine.installation_id=$2 AND e.installation_id<>$2 AND mine.accepted_at>$3-86400)",
                  [window_id, id, at]
                )

              query(
                "INSERT INTO issued_bundles(installation_id,bundle_id,issued_at) VALUES($1,$2,$3) ON CONFLICT DO NOTHING",
                [id, window_id, at]
              )

              %{
                id: window_id,
                starts_at: window_id * 60,
                ends_at: (window_id + 1) * 60,
                expires_at: (window_id + 1) * 60 + 120,
                drops: drops,
                echoes: echoes,
                has_other: other,
                echo_answer: answer
              }

            [] ->
              nil
          end

        # Cursor advances over empty minutes too; no history replay on the next poll.
        %{
          version: 1,
          server_time: at,
          cursor: encode_cursor(id, div(at, 60) - 1),
          bundle: bundle,
          latest_minute_active:
            query("SELECT id FROM bundles WHERE id=$1", [div(at, 60) - 1]) != []
        }
      end)
    end
  end

  defp encode_cursor(id, window_id),
    do: Phoenix.Token.sign(PondWeb.Endpoint, "pond-cursor", {id, window_id})

  defp decode_cursor(_, nil), do: {:ok, -1}

  defp decode_cursor(id, token) when is_binary(token) and byte_size(token) < 512 do
    case Phoenix.Token.verify(PondWeb.Endpoint, "pond-cursor", token, max_age: 86400) do
      {:ok, {^id, window_id}} when is_integer(window_id) -> {:ok, window_id}
      _ -> {:error, :invalid_cursor}
    end
  end

  defp decode_cursor(_, _), do: {:error, :invalid_cursor}

  def leave(credential, at \\ now()) do
    with {:ok, id, _} <- identity(credential) do
      Repo.transaction(fn ->
        query("SELECT pg_advisory_xact_lock(7061)")
        query("SELECT pg_advisory_xact_lock(hashtextextended($1, 0))", [Base.encode16(id)])
        # An uncertain bootstrap may arrive after leave. Revoke that identity too.
        existing = query("SELECT id FROM installations WHERE id=$1 FOR UPDATE", [id]) != []
        deleted = query("SELECT id FROM deletions WHERE id=$1", [id]) != []
        [[count]] = query("SELECT count(*) FROM deletions")
        if not existing and not deleted and count >= 100_000, do: Repo.rollback(:capacity)

        query("INSERT INTO deletions(id,deleted_at) VALUES($1,$2) ON CONFLICT DO NOTHING", [
          id,
          at
        ])

        query("DELETE FROM installations WHERE id=$1", [id])
        %{deleted: true, version: 1}
      end)
    end
  end

  def cleanup(at \\ now()) do
    Repo.transaction(fn ->
      query("DELETE FROM drops WHERE accepted_at<=$1-86400", [at])
      query("DELETE FROM issued_bundles WHERE issued_at<=$1-86400", [at])
      query("DELETE FROM bundles WHERE (id+1)*60<=$1-86400", [at])
      query("DELETE FROM installations WHERE last_seen<=$1-2592000", [at])
      query("DELETE FROM deletions WHERE deleted_at<=$1-691200", [at])
    end)
  end
end
