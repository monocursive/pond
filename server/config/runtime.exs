import Config

if config_env() != :test && System.get_env("DATABASE_URL") do
  config :pond, Pond.Repo, url: System.fetch_env!("DATABASE_URL")
end

if config_env() == :prod do
  config :pond, :trust_kamal_proxy, System.get_env("TRUST_KAMAL_PROXY") == "true"
  config :pond, Pond.Repo, url: System.fetch_env!("DATABASE_URL"), pool_size: 10

  config :pond, PondWeb.Endpoint,
    url: [host: System.fetch_env!("PHX_HOST"), scheme: "https", port: 443],
    http: [ip: {0, 0, 0, 0}, port: String.to_integer(System.get_env("PORT", "4000"))],
    secret_key_base: System.fetch_env!("SECRET_KEY_BASE")
end
