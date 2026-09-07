# Pond

A little shared pond for people wearing Pebble watches.

Drop a pebble. Somewhere, another wearer may feel a ripple. Nothing needs answering.

Pond is a community gift by Michaël / Monocursive, intended to be free and open source. This monorepo now contains the first implementation: a **C watchface**, **PebbleKit JS bridge**, and **Elixir / Phoenix server** backed by PostgreSQL. It is a local prototype; physical testing and public deployment are still ahead.

## Run the server

Requires Elixir/OTP, PostgreSQL, and the standard Pebble SDK for watch builds. Development was checked with Elixir 1.20.2 / OTP 29, PostgreSQL 17, Pebble Tool 5.0.40 / SDK 4.33.1, Node 22, and a C compiler. Mix dependencies are locked in `server/mix.lock`; the watch has no third-party JavaScript dependencies.

From the repository root:

```sh
docker compose up -d db
cd server
mix setup
mix phx.server
```

Phoenix listens at [localhost:4040](http://127.0.0.1:4040/health). The [settings page](http://127.0.0.1:4040/settings.html) can be previewed in a browser; saving/joining returns to the Pebble app through its settings webview. PostgreSQL is isolated on loopback port 55432 with development-only credentials. An existing PostgreSQL installation works too: set `DATABASE_URL` for development. Tests use the separate `pond_test` database on the same local server.

## Build the watchface

From the repository root:

```sh
cd watch
pebble build
pebble install --emulator aplite
```

The bundle is `watch/build/watch.pbw`, compiled for Aplite, Basalt, Chalk, Diorite, Flint, Emery and Gabbro. Use `pebble install --phone <phone-ip>` for a physical phone with its developer connection enabled. Before physical use, set `watch/src/pkjs/env.js` to your reachable **HTTPS origin** and rebuild. The checked-in loopback URL is for local development; `127.0.0.1` on a phone points to the phone itself.

The face starts locally, unjoined and silent. Open Pond's settings in the Pebble mobile app to join. Save settings, then reopen to inspect the watch acknowledgement. Two deliberate taps prepare a drop; another double-tap within two seconds cancels it. Gesture thresholds remain experimental. Switching away from Pond stops its watch-side activity.

## Check the implementation

From `server/`:

```sh
mix format --check-formatted
mix test
mix compile --warnings-as-errors
```

From the repository root:

```sh
node --test watch/test/*.test.js
cc -std=c11 -Wall -Wextra -Werror watch/src/c/policy.c watch/test/policy_test.c -o /tmp/pond-policy-test
/tmp/pond-policy-test
cc -std=c11 -Wall -Wextra -Werror watch/src/c/gesture.c watch/test/gesture_test.c -o /tmp/pond-gesture-test
/tmp/pond-gesture-test
cc -std=c11 -Wall -Wextra -Werror -Iwatch/test/stubs watch/src/c/drop_outbox.c watch/test/drop_outbox_test.c -o /tmp/pond-outbox-test
/tmp/pond-outbox-test
```

Then run `pebble build` from `watch/`. No root task runner or package manager workspace is needed.

## Server release

From `server/`, `MIX_ENV=prod mix release` builds a normal OTP release. Supply `DATABASE_URL`, `SECRET_KEY_BASE` and `PHX_HOST` as shown in `.env.example`; `mix phx.gen.secret` generates a signing secret. Terminate HTTPS at your deployment platform or reverse proxy, and keep PostgreSQL private.

```sh
_build/prod/rel/pond/bin/pond eval 'Pond.Release.migrate()'
_build/prod/rel/pond/bin/pond start
```

Migrations must run before starting the server. Backup/deletion restoration, hosting logs and physical comfort gates must be verified before public operation. This repository has not been deployed.

## Product and design

- [Architecture: events and database](docs/ARCHITECTURE.md) — diagrams and editable FigJam board.
- [Implementation status and remaining gates](docs/IMPLEMENTATION.md)
- [Protocol v1](docs/PROTOCOL.md)
- [First release](docs/FIRST-RELEASE.md) — implementation scope and release gates.
- [North star](docs/NORTH-STAR.md) — longer-term vision.
- [Original v0.2](docs/archive/PRODUCT-SPEC-v0.2.md) — preserved verbatim.
- [UI/UX design](design/DESIGN-HANDOFF.md) — editable Figma screens and exported artwork.

The first-release specification takes precedence over the north star. Later ideas are possibilities, not promised features. The [implementation status](docs/IMPLEMENTATION.md) records what the current prototype proves and what it does not.
