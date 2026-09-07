defmodule PondWeb.Landing do
  @moduledoc "The public introduction is static and independent of API quotas."
  import Plug.Conn

  def init(opts), do: opts

  def call(%{method: method, request_path: "/"} = conn, _) when method in ["GET", "HEAD"] do
    path = Application.app_dir(:pond, "priv/static/landing/index.html")

    conn
    |> put_resp_content_type("text/html")
    |> put_resp_header("x-content-type-options", "nosniff")
    |> put_resp_header("referrer-policy", "strict-origin-when-cross-origin")
    |> put_resp_header(
      "content-security-policy",
      "default-src 'self'; object-src 'none'; base-uri 'none'; frame-ancestors 'none'"
    )
    |> send_file(200, path)
    |> halt()
  end

  def call(conn, _), do: conn
end
