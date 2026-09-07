# Pond on the VPS

Pond runs at https://pond.monocursive.com on `ubuntu@pond.monocursive.com` (Ubuntu 26.04, amd64). Cloudflare DNS is **DNS only**. Kamal 2.12 manages one Phoenix container, a private Postgres 17 accessory, and the HTTPS proxy. The watch's default service origin points to this hostname.

Verified on 2026-09-07: active version `pond-20260907-2`. A second Kamal deployment completed with all 13 sampled HTTPS readiness requests returning success. The previous app container is retained for rollback. All 19 server tests and 12 bridge tests pass; the watch bundle builds for all seven targets.

## Deploy from this checkout

Install Ruby, Docker, and the locked gems with `bundle install`. Docker must be running locally. SSH key access to the VPS is required. The remote builder compiles amd64 images on the VPS; Kamal's local registry transfers them through SSH and is bound to local loopback, so no external registry credentials are needed.

Copy `.kamal/secrets.example` to `.kamal/secrets` for a new installation, replace its placeholders, and set permissions to `600`. **For this existing deployment, retain the already-generated secrets.** Changing the password environment variable does not change an existing Postgres user's password. Secrets are ignored by Git and excluded from Docker's build context.

From the repository root:

```sh
bundle exec kamal deploy
```

`builder.context: .` deliberately includes the current working files. Commit changes before ordinary releases so image tags identify reproducible source. Initial deployment used explicit `pond-20260907-*` tags while the authorized landing/deployment changes were uncommitted. `kamal setup` is for the first setup; normal deploys leave the database accessory running.

The image assembles an OTP release and runs as `nobody`. Its startup script runs the migration entry point before starting Phoenix. A failed migration prevents traffic switching. `/ready` checks the migrated database; `/health` is liveness only. Both bypass API quotas. Maintain backward-compatible migrations because the old and new app briefly overlap. App rollback does not roll back database schema:

```sh
bundle exec kamal app details
bundle exec kamal app logs
bundle exec kamal rollback PREVIOUS_VERSION
```

Finalization is serialized by a Postgres advisory transaction lock, while cleanup is transactional and idempotent. A brief deployment overlap can duplicate work but must not duplicate bundles. Running multiple permanent app instances would also multiply the in-memory ingress allowance; this deployment intentionally uses one app instance.

## Network and privacy

Only SSH, HTTP, and HTTPS are publicly exposed. Postgres and the app port are internal to Docker's `kamal` network. The app's proxy trust is enabled only for this topology. Kamal replaces incoming forwarded headers (`forward_headers: false`), and Pond uses the resulting client address for its ephemeral rate limiter. Do not expose port 4000 publicly or enable Cloudflare proxying without revisiting this trust boundary.

Kamal's automatic certificates depend on direct access to the proxy. The proxy uses Docker's `none` logging driver because its built-in access records include client IPs. This also removes persisted proxy diagnostic logs; inspect container status and HTTPS readiness for failures. App logs rotate at 10 MB × 7 files. Backups and operational alerts remain separate from Kamal.

## Backups

`deploy/backup` is installed as `/usr/local/sbin/pond-backup`. `pond-backup.timer` runs it daily around 03:17 UTC, catches up after downtime, and retains encrypted dumps for approximately seven days on the VPS. Dumps use `pg_dump -Fc` and age encryption; no decryption key is stored on the server. Check runs with:

```sh
ssh ubuntu@pond.monocursive.com 'sudo systemctl status pond-backup.timer; sudo journalctl -u pond-backup.service --since yesterday'
```

The initial encrypted dump was copied off the VPS and decrypted/restored successfully into a temporary database, which was then removed. On the deployment Mac, the owner-only key and first copies live in `~/.config/pond/`. Preserve the key separately; encrypted dumps cannot be recovered without it.

**Remaining:** automated offsite upload and failure alerts need a durable storage/notification destination. Daily local dumps alone do not protect against loss of the VPS. Before restoring live traffic from an older backup, reconcile the newest eight-day deletion tombstones from an independent copy, as required by the release spec; this continuous offsite deletion journal is not configured yet.

## Release boundary

HTTPS, landing/settings pages, database readiness, and a two-installation register → drop → ripple → echo → delete cycle have been verified against the live service. These are synthetic API clients, not physical watches. The seven-platform PBW has the public origin; it has not been uploaded to the Pebble Store. Physical gesture, background phone delivery, comfort and battery validation remain required.
