defmodule PondWeb.Throttle do
  @moduledoc "Bounded, ephemeral ingress limits. No IPs or credentials are logged or persisted."
  use GenServer
  import Plug.Conn
  def start_link(_opts), do: GenServer.start_link(__MODULE__, :server, name: __MODULE__)

  def init(:server),
    do: {:ok, %{minute: -1, entries: %{}, total: 0, salt: :crypto.strong_rand_bytes(32)}}

  def init(opts), do: opts

  # Liveness must remain observable even when the API allowance is exhausted.
  def call(%{method: "GET", request_path: "/health"} = conn, _), do: conn
  def call(%{method: "GET", request_path: "/ready"} = conn, _), do: conn

  def call(conn, _) do
    category = if conn.request_path == "/v1/installations", do: :join, else: :api

    ip_allowed = GenServer.call(__MODULE__, {:allow, conn.remote_ip, category})

    credential_allowed =
      ip_allowed and
        case get_req_header(conn, "authorization") do
          ["Bearer " <> credential] when byte_size(credential) == 64 ->
            GenServer.call(__MODULE__, {:allow, credential, :credential})

          _ ->
            true
        end

    case ip_allowed and credential_allowed do
      true ->
        conn

      false ->
        conn
        |> put_resp_content_type("application/json")
        |> put_resp_header("retry-after", "60")
        |> send_resp(429, ~s({"error":"rate_limited"}))
        |> halt()
    end
  end

  def handle_call({:allow, ip, category}, _, state) do
    minute = div(System.monotonic_time(:second), 60)

    state =
      if minute != state.minute,
        do: %{state | minute: minute, entries: %{}, total: 0},
        else: state

    key = :crypto.mac(:hmac, :sha256, state.salt, :erlang.term_to_binary({ip, category}))
    count = Map.get(state.entries, key, 0)

    limit =
      case category do
        :join -> 10
        :credential -> 60
        :api -> 180
      end

    allowed = count < limit and state.total < 3000

    state =
      if allowed,
        do: %{state | total: state.total + 1, entries: Map.put(state.entries, key, count + 1)},
        else: state

    {:reply, allowed, state}
  end
end
