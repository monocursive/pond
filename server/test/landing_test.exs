defmodule PondWeb.LandingTest do
  use ExUnit.Case, async: true
  import Plug.Conn
  import Phoenix.ConnTest
  @endpoint PondWeb.Endpoint

  test "the root serves HTML outside the API pipeline and quota" do
    conn = build_conn() |> put_req_header("accept", "text/html") |> get("/")
    assert html_response(conn, 200) =~ "A little plop."

    assert get_resp_header(conn, "content-security-policy") == [
             "default-src 'self'; object-src 'none'; base-uri 'none'; frame-ancestors 'none'"
           ]

    # The landing plug halts before the throttle, even without an API identity.
    direct = Plug.Test.conn(:get, "/") |> PondWeb.Landing.call([])
    assert direct.halted
    assert direct.status == 200
    assert head(build_conn(), "/").status == 200
  end

  test "landing assets and existing settings are served, and the API remains JSON" do
    for path <-
          ~w(/landing/pond.css /landing/pond.js /landing/stone.svg /landing/fraunces.ttf /landing/watch-still.png /landing/watch-ripple.png /settings.html /settings.js /settings.css) do
      assert get(build_conn(), path).status == 200, path
    end

    assert json_response(get(build_conn(), "/health"), 200)["status"] == "ok"
    untouched = Plug.Test.conn(:get, "/v1/pond") |> PondWeb.Landing.call([])
    refute untouched.halted
  end
end
