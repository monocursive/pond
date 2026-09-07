defmodule PondWeb.ClientIP do
  @moduledoc "Opt-in for the private Kamal network; the app must have no public port."
  import Plug.Conn
  def init(opts), do: opts

  def call(conn, _opts) do
    if Application.get_env(:pond, :trust_kamal_proxy, false) do
      forwarded(conn)
    else
      conn
    end
  end

  # Kamal replaces untrusted incoming forwarding headers with forward_headers:
  # false. Also choose the last hop defensively, never a client-supplied prefix.
  def forwarded(conn) do
    value =
      conn
      |> get_req_header("x-forwarded-for")
      |> Enum.join(",")
      |> String.split(",")
      |> List.last()
      |> String.trim()

    case :inet.parse_address(String.to_charlist(value)) do
      {:ok, ip} -> %{conn | remote_ip: ip}
      _ -> conn
    end
  end
end
