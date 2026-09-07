defmodule Pond.Release do
  @moduledoc "Database migration entry point for a Mix release."
  def migrate do
    Application.load(:pond)
    {:ok, _, _} = Ecto.Migrator.with_repo(Pond.Repo, &Ecto.Migrator.run(&1, :up, all: true))
    :ok
  end
end
