defmodule Pond.Aggregator do
  use GenServer
  def start_link(opts), do: GenServer.start_link(__MODULE__, opts, name: __MODULE__)
  def init(_), do: {:ok, 0, {:continue, :tick}}
  def handle_continue(:tick, count), do: tick(count)
  def handle_info(:tick, count), do: tick(count)

  defp tick(count) do
    Pond.finalize()
    if rem(count, 60) == 0, do: Pond.cleanup()
    Process.send_after(self(), :tick, 60_000)
    {:noreply, count + 1}
  end
end
