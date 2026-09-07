defmodule PondWeb.ThrottleTest do
  use ExUnit.Case, async: false
  alias PondWeb.Throttle

  setup do
    previous = :sys.get_state(Throttle)
    {:ok, fresh} = Throttle.init(:server)
    :sys.replace_state(Throttle, fn _ -> fresh end)
    on_exit(fn -> :sys.replace_state(Throttle, fn _ -> previous end) end)
    :ok
  end

  test "rotating credentials from a blocked IP cannot spend the global allowance" do
    for n <- 1..3000 do
      credential = n |> Integer.to_string(16) |> String.pad_leading(64, "0")

      Plug.Test.conn("GET", "/v1/pond")
      |> Plug.Conn.put_req_header("authorization", "Bearer " <> credential)
      |> Throttle.call([])
    end

    assert :sys.get_state(Throttle).total <= 360

    other =
      Plug.Test.conn("GET", "/v1/pond")
      |> Map.put(:remote_ip, {203, 0, 113, 10})
      |> Throttle.call([])

    refute other.halted
  end

  test "health remains available without consuming quota when the allowance is exhausted" do
    :sys.replace_state(Throttle, fn state ->
      %{state | total: 3000, minute: div(System.monotonic_time(:second), 60)}
    end)

    conn = Plug.Test.conn("GET", "/health") |> Throttle.call([])
    refute conn.halted
    assert :sys.get_state(Throttle).total == 3000
  end
end
