defmodule Pond.ConcurrencyTest do
  use ExUnit.Case, async: false
  alias Ecto.Adapters.SQL.Sandbox
  defp db(fun), do: Sandbox.unboxed_run(Pond.Repo, fun)

  test "independent database connections serialize duplicate and competing drops" do
    secret = :crypto.strong_rand_bytes(32) |> Base.encode16(case: :lower)
    at = Pond.now()
    {:ok, _} = db(fn -> Pond.register(secret, at) end)
    {:ok, id} = db(fn -> Pond.authenticate(secret, at) end)
    on_exit(fn -> db(fn -> Pond.leave(secret, at) end) end)
    request = Ecto.UUID.generate()

    results =
      1..8
      |> Task.async_stream(fn _ -> db(fn -> Pond.drop(id, request, nil, at) end) end,
        max_concurrency: 8
      )
      |> Enum.map(fn {:ok, r} -> r end)

    assert Enum.count(results, &match?({:ok, %{duplicate: false}}, &1)) == 1
    assert Enum.count(results, &match?({:ok, %{duplicate: true}}, &1)) == 7

    competing =
      1..8
      |> Task.async_stream(
        fn _ -> db(fn -> Pond.drop(id, Ecto.UUID.generate(), nil, at + 30) end) end,
        max_concurrency: 8
      )
      |> Enum.map(fn {:ok, r} -> r end)

    assert Enum.count(competing, &match?({:ok, _}, &1)) == 1
    assert Enum.count(competing, &match?({:error, :rate_limited}, &1)) == 7
  end
end
