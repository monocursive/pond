import Config
config :pond, ecto_repos: [Pond.Repo]
config :phoenix, :json_library, Jason
config :pond, Pond.Repo, log: false

config :pond, PondWeb.Endpoint,
  adapter: Bandit.PhoenixAdapter,
  render_errors: [formats: [json: PondWeb.ErrorJSON], layout: false],
  pubsub_server: Pond.PubSub

config :phoenix, :filter_parameters, ["credential", "authorization", "request_id"]
config :logger, level: :warning
import_config "#{config_env()}.exs"
