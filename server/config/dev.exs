import Config
config :pond, Pond.Repo, url: "postgres://pond:pond@localhost:55432/pond_dev", pool_size: 10

config :pond, PondWeb.Endpoint,
  http: [ip: {127, 0, 0, 1}, port: 4040],
  secret_key_base: String.duplicate("local-development-only-", 4),
  server: true
