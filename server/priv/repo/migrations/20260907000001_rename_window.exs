defmodule Pond.Repo.Migrations.RenameWindow do
  use Ecto.Migration
  def change, do: rename(table(:drops), :window, to: :window_id)
end
