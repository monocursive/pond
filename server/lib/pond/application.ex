defmodule Pond.Application do
  use Application

  def start(_type, _args) do
    children =
      [Pond.Repo, {Phoenix.PubSub, name: Pond.PubSub}, PondWeb.Throttle] ++
        if(Application.get_env(:pond, :background_jobs, true), do: [Pond.Aggregator], else: []) ++
        [PondWeb.Endpoint]

    Supervisor.start_link(children, strategy: :one_for_one, name: Pond.Supervisor)
  end
end
