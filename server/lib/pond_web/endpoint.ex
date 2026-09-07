defmodule PondWeb.Endpoint do
  use Phoenix.Endpoint, otp_app: :pond
  plug(PondWeb.Landing)

  plug(Plug.Static,
    at: "/",
    from: :pond,
    only: ~w(landing settings.html settings.js settings.css)
  )

  plug(PondWeb.ClientIP)
  plug(PondWeb.Throttle)

  plug(Plug.Parsers,
    parsers: [:json],
    pass: ["application/json"],
    json_decoder: Jason,
    length: 4096,
    read_length: 4096
  )

  plug(PondWeb.Router)
end
