FROM hexpm/elixir:1.20.2-erlang-29.0.3-debian-trixie-20260623-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends build-essential git ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /app
ENV MIX_ENV=prod
RUN mix local.hex --force && mix local.rebar --force
COPY server/mix.exs server/mix.lock ./
COPY server/config/config.exs server/config/prod.exs config/
RUN mix deps.get --only prod && mix deps.compile
COPY server/lib lib
COPY server/priv priv
COPY server/config/runtime.exs config/
RUN mix compile --warnings-as-errors && mix release

FROM debian:trixie-20260623-slim
RUN apt-get update && apt-get install -y --no-install-recommends libstdc++6 openssl libncurses6 ca-certificates && rm -rf /var/lib/apt/lists/*
ENV LANG=C.UTF-8 MIX_ENV=prod
WORKDIR /app
COPY --from=build --chown=nobody:root /app/_build/prod/rel/pond ./
COPY --chmod=755 server/rel/start /app/start
USER nobody
EXPOSE 4000
CMD ["/app/start"]
