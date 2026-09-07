defmodule PondWeb.APIController do
  use Phoenix.Controller, formats: [:json]
  def health(conn, _), do: json(conn, %{status: "ok", version: 1})

  def ready(conn, _) do
    case Pond.Repo.query("SELECT 1 FROM schema_migrations LIMIT 1", [], timeout: 2_000) do
      {:ok, %{num_rows: 1}} -> json(conn, %{status: "ready"})
      _ -> conn |> put_status(503) |> json(%{status: "unavailable"})
    end
  rescue
    _ -> conn |> put_status(503) |> json(%{status: "unavailable"})
  catch
    :exit, _ -> conn |> put_status(503) |> json(%{status: "unavailable"})
  end

  def register(conn, %{"version" => 1} = params) when map_size(params) == 1,
    do: respond(conn, Pond.register(token(conn)))

  def register(conn, _), do: respond(conn, {:error, :invalid_request})

  def drop(conn, %{"version" => 1, "request_id" => request} = params) do
    with true <- Enum.all?(Map.keys(params), &(&1 in ["version", "request_id", "source_bundle"])),
         {:ok, id} <- Pond.authenticate(token(conn)) do
      respond(conn, Pond.drop(id, request, params["source_bundle"]))
    else
      false -> respond(conn, {:error, :invalid_request})
      error -> respond(conn, error)
    end
  end

  def drop(conn, _), do: respond(conn, {:error, :invalid_request})

  def pond(conn, params) do
    case Pond.authenticate(token(conn)) do
      {:ok, id} -> respond(conn, Pond.pond(id, params["after"]))
      error -> respond(conn, error)
    end
  end

  def leave(conn, _), do: respond(conn, Pond.leave(token(conn)))

  defp token(conn) do
    case get_req_header(conn, "authorization") do
      ["Bearer " <> credential] -> credential
      _ -> nil
    end
  end

  defp respond(conn, {:ok, data}),
    do: conn |> put_resp_header("cache-control", "no-store") |> json(data)

  defp respond(conn, {:error, reason}) do
    status =
      case reason do
        :unauthorized -> 401
        :revoked -> 410
        :capacity -> 503
        :rate_limited -> 429
        _ -> 422
      end

    conn |> put_status(status) |> json(%{error: reason, version: 1})
  end
end
