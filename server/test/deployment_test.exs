defmodule PondWeb.DeploymentTest do
  use ExUnit.Case, async: false
  import Plug.Conn
  import Phoenix.ConnTest
  @endpoint PondWeb.Endpoint

  test "proxy IP trust is disabled by default and takes only the nearest forwarded hop" do
    conn = Plug.Test.conn(:get, "/") |> put_req_header("x-forwarded-for", "1.2.3.4, 203.0.113.8")
    assert PondWeb.ClientIP.call(conn, []).remote_ip == conn.remote_ip
    assert PondWeb.ClientIP.forwarded(conn).remote_ip == {203, 0, 113, 8}
    malformed = conn |> put_req_header("x-forwarded-for", "1.2.3.4, invalid")
    assert PondWeb.ClientIP.forwarded(malformed).remote_ip == conn.remote_ip
  end

  test "readiness checks the migrated database" do
    :ok = Ecto.Adapters.SQL.Sandbox.checkout(Pond.Repo)
    assert json_response(get(build_conn(), "/ready"), 200)["status"] == "ready"
  end
end
