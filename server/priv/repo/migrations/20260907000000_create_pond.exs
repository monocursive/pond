defmodule Pond.Repo.Migrations.CreatePond do
  use Ecto.Migration

  def change do
    create table(:installations, primary_key: false) do
      add(:id, :uuid, primary_key: true)
      add(:credential_hash, :binary, null: false)
      add(:last_seen, :bigint, null: false)
    end

    create(unique_index(:installations, [:credential_hash]))
    create(index(:installations, [:last_seen]))

    create table(:drops, primary_key: false) do
      add(:installation_id, references(:installations, type: :uuid, on_delete: :delete_all),
        primary_key: true
      )

      add(:request_id, :uuid, primary_key: true)
      add(:accepted_at, :bigint, null: false)
      add(:window, :bigint, null: false)
      add(:source_bundle, :bigint)
    end

    create(index(:drops, [:window]))
    create(index(:drops, [:accepted_at]))
    create(index(:drops, [:source_bundle]))

    create table(:bundles, primary_key: false) do
      add(:id, :bigint, primary_key: true)
      add(:drops, :integer, null: false)
      add(:echoes, :integer, null: false)
    end

    create table(:issued_bundles, primary_key: false) do
      add(:installation_id, references(:installations, type: :uuid, on_delete: :delete_all),
        primary_key: true
      )

      add(:bundle_id, :bigint, primary_key: true)
      add(:issued_at, :bigint, null: false)
    end

    create(index(:issued_bundles, [:issued_at]))

    create table(:deletions, primary_key: false) do
      add(:id, :uuid, primary_key: true)
      add(:deleted_at, :bigint, null: false)
    end
  end
end
