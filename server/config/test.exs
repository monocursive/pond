import Config
config :pond, :background_jobs, false

config :pond, Pond.Repo,
  url: "postgres://pond:pond@localhost:55432/pond_test",
  pool: Ecto.Adapters.SQL.Sandbox,
  pool_size: 10

config :pond, PondWeb.Endpoint,
  server: false,
  secret_key_base: String.duplicate("test-only-", 8)
