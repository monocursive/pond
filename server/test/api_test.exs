defmodule PondWeb.APITest do
  use ExUnit.Case, async: false
  import Plug.Conn
  import Phoenix.ConnTest
  @endpoint PondWeb.Endpoint
  setup do
    :ok = Ecto.Adapters.SQL.Sandbox.checkout(Pond.Repo)
    :ok
  end

  test "HTTP registration, accepted drop, poll and reconciled deletion" do
    secret = :crypto.strong_rand_bytes(32) |> Base.encode16(case: :lower)
    conn = build_conn() |> put_req_header("authorization", "Bearer " <> secret)
    assert json_response(post(conn, "/v1/installations", %{version: 1}), 200)["id"]
    request = Ecto.UUID.generate()

    assert json_response(post(conn, "/v1/drops", %{version: 1, request_id: request}), 200)[
             "request_id"
           ] == request

    assert json_response(get(conn, "/v1/pond"), 200)["cursor"]
    assert json_response(delete(conn, "/v1/installation"), 200)["deleted"]
    assert json_response(delete(conn, "/v1/installation"), 200)["deleted"]
    assert json_response(get(conn, "/v1/pond"), 401)["error"] == "unauthorized"
  end

  test "strict bootstrap schema and no unauthenticated activity" do
    assert json_response(post(build_conn(), "/v1/installations", %{version: 2}), 422)["error"]
    assert json_response(get(build_conn(), "/v1/pond"), 401)["error"]
    assert json_response(get(build_conn(), "/health"), 200)["status"] == "ok"
  end
end
