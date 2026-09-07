defmodule PondWeb.Endpoint do
  use Phoenix.Endpoint, otp_app: :pond
  plug(Plug.Static, at: "/", from: :pond, only: ~w(settings.html settings.js settings.css))
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
