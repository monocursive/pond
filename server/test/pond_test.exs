defmodule PondTest do
  use ExUnit.Case, async: false
  alias Pond.Repo

  setup do
    :ok = Ecto.Adapters.SQL.Sandbox.checkout(Repo)
    %{at: div(Pond.now(), 60) * 60 + 10}
  end

  defp user(at) do
    secret = :crypto.strong_rand_bytes(32) |> Base.encode16(case: :lower)
    {:ok, _} = Pond.register(secret, at)
    {:ok, id} = Pond.authenticate(secret, at)
    {secret, id}
  end

  test "bootstrap retries keep one identity; deletion cannot be undone by a retry", %{at: at} do
    {secret, _} = user(at)
    assert {:ok, first} = Pond.register(secret, at)
    assert {:ok, ^first} = Pond.register(secret, at)
    assert Repo.query!("SELECT count(*) FROM installations").rows == [[1]]
    assert {:ok, %{deleted: true}} = Pond.leave(secret, at)
    assert {:ok, %{deleted: true}} = Pond.leave(secret, at + 1)
    assert {:error, :revoked} = Pond.register(secret, at + 1)
    assert {:error, :unauthorized} = Pond.authenticate(secret, at + 1)
  end

  test "leaving before a delayed bootstrap prevents later resurrection", %{at: at} do
    secret = :crypto.strong_rand_bytes(32) |> Base.encode16(case: :lower)
    assert {:ok, %{deleted: true}} = Pond.leave(secret, at)
    assert {:error, :revoked} = Pond.register(secret, at + 1)
  end

  test "idempotency precedes limits and returns original classification", %{at: at} do
    {_, id} = user(at)
    request = Ecto.UUID.generate()
    assert {:ok, %{kind: "drop", duplicate: false}} = Pond.drop(id, request, nil, at)
    assert {:ok, %{kind: "drop", duplicate: true}} = Pond.drop(id, request, 999, at + 1)
    assert {:error, :rate_limited} = Pond.drop(id, Ecto.UUID.generate(), nil, at + 29)
    assert {:ok, _} = Pond.drop(id, Ecto.UUID.generate(), nil, at + 30)
  end

  test "rolling daily contribution ceiling", %{at: at} do
    {_, id} = user(at)
    for n <- 0..39, do: assert({:ok, _} = Pond.drop(id, Ecto.UUID.generate(), nil, at + n * 30))
    assert {:error, :rate_limited} = Pond.drop(id, Ecto.UUID.generate(), nil, at + 1200)
    assert {:ok, _} = Pond.drop(id, Ecto.UUID.generate(), nil, at + 86400)
  end

  test "drop, issued ripple, echo, and truthful bundle answer", %{at: at} do
    {_, alice} = user(at)
    {_, bob} = user(at)
    {:ok, drop} = Pond.drop(alice, Ecto.UUID.generate(), nil, at)
    {:ok, ripple} = Pond.pond(bob, nil, at + 60)
    assert ripple.bundle.id == drop.bundle_id
    assert ripple.bundle.has_other

    assert {:ok, %{kind: "echo"}} =
             Pond.drop(bob, Ecto.UUID.generate(), ripple.bundle.id, at + 61)

    {:ok, answer} = Pond.pond(alice, nil, at + 120)
    assert answer.bundle.echo_answer
    assert answer.bundle.echoes == 1
  end

  test "self-only, unissued and expired sources become plain drops", %{at: at} do
    {_, alice} = user(at)
    {_, bob} = user(at)
    {:ok, drop} = Pond.drop(alice, Ecto.UUID.generate(), nil, at)
    {:ok, own} = Pond.pond(alice, nil, at + 60)
    refute own.bundle.has_other

    assert {:ok, %{kind: "drop"}} =
             Pond.drop(alice, Ecto.UUID.generate(), drop.bundle_id, at + 61)

    assert {:ok, %{kind: "drop"}} = Pond.drop(bob, Ecto.UUID.generate(), drop.bundle_id, at + 61)
    {:ok, _} = Pond.pond(bob, nil, at + 62)
    assert {:ok, %{kind: "drop"}} = Pond.drop(bob, Ecto.UUID.generate(), drop.bundle_id, at + 181)
  end

  test "cursor ownership, latest-only delivery and expiry", %{at: at} do
    {_, alice} = user(at)
    {_, bob} = user(at)
    {:ok, _} = Pond.drop(alice, Ecto.UUID.generate(), nil, at)
    {:ok, first} = Pond.pond(bob, nil, at + 60)
    assert {:error, :invalid_cursor} = Pond.pond(alice, first.cursor, at + 61)
    assert {:ok, %{bundle: nil}} = Pond.pond(bob, first.cursor, at + 61)
    assert {:ok, %{bundle: nil}} = Pond.pond(bob, nil, at + 180)
    assert {:error, :invalid_cursor} = Pond.pond(bob, "bad", at)
  end

  test "finalisation is immutable and survives repeated runs", %{at: at} do
    {secret, id} = user(at)
    {:ok, _} = Pond.drop(id, Ecto.UUID.generate(), nil, at)
    Pond.finalize(at + 60)
    before = Repo.query!("SELECT * FROM bundles").rows
    Pond.finalize(at + 120)
    assert Repo.query!("SELECT * FROM bundles").rows == before
    Pond.leave(secret, at + 121)
    Pond.finalize(at + 122)
    assert Repo.query!("SELECT * FROM bundles").rows == before
    assert Repo.query!("SELECT count(*) FROM drops").rows == [[0]]
  end

  test "retention and inactive identity expiry apply without waiting for cleanup", %{at: at} do
    {secret, id} = user(at)
    {:ok, _} = Pond.drop(id, Ecto.UUID.generate(), nil, at)
    Pond.finalize(at + 60)
    assert {:error, :unauthorized} = Pond.authenticate(secret, at + 2_592_001)
    Pond.cleanup(at + 2_592_001)

    for table <- ~w(installations drops bundles issued_bundles) do
      assert Repo.query!("SELECT count(*) FROM #{table}").rows == [[0]]
    end
  end

  test "invalid request identifiers and schemas fail cleanly", %{at: at} do
    {_, id} = user(at)
    assert {:error, :invalid_request} = Pond.drop(id, "invalid", nil, at)
    assert {:error, :invalid_request} = Pond.drop(id, Ecto.UUID.generate(), "not-a-bundle", at)
    assert {:error, :unauthorized} = Pond.register("short", at)
  end
end
