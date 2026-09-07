defmodule Pond.Repo do
  use Ecto.Repo, otp_app: :pond, adapter: Ecto.Adapters.Postgres
end
