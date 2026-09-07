defmodule Pond.MixProject do
  use Mix.Project

  def project do
    [
      app: :pond,
      version: "0.1.0",
      elixir: "~> 1.17",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      aliases: [
        setup: ["deps.get", "ecto.create", "ecto.migrate"],
        test: ["ecto.create --quiet", "ecto.migrate --quiet", "test"]
      ]
    ]
  end

  def application, do: [mod: {Pond.Application, []}, extra_applications: [:logger, :crypto]]

  defp deps do
    [
      {:phoenix, "~> 1.8.13"},
      {:ecto_sql, "~> 3.14"},
      {:postgrex, "~> 0.22"},
      {:bandit, "~> 1.8"},
      {:jason, "~> 1.4"}
    ]
  end
end
