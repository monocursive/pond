# Architecture: events and database

This describes the first implementation, checked against the source on 7 September 2026.

[Open the editable FigJam board](https://www.figma.com/board/tvOnb6475CcjAhz8TAqeVt).

## Responsibilities

| Component | Responsibility | Persistent state |
| --- | --- | --- |
| C watchface | Gesture, local time and visuals, settings acknowledgement, final haptic policy | Avatar/settings, deduplication and rolling incoming vibration budget |
| ES5 PebbleKit JS | AppMessage/HTTPS bridge, credential, bounded request retries, minute polling | Per-origin credential, request UUID/pending delivery, cursor, desired settings/revision |
| Phoenix | Authenticate, validate contributions, aggregate completed minutes, return recipient context | Uses Ecto/PostgreSQL; one OTP aggregator process |
| PostgreSQL | Transactional acceptance, idempotency, immutable minute totals, eligibility and revocation | Five tables below |

The phone sends commands and polls shared state. These are application events and database records, not a message-broker architecture. Phoenix has no event bus or WebSocket delivery service.

## From a drop to a ripple

Watch A and Watch B represent two wearers. Each phone runs the same bridge. The first request, reconnect, failure recovery and long-resume paths fetch a silent snapshot; they do not replay incoming haptics.

```mermaid
sequenceDiagram
    title Pond - From a deliberate drop to a shared ripple
    participant WatchA
    participant PhoneA
    participant PhoenixAPI
    participant PostgreSQL
    participant PhoneB
    participant WatchB
    WatchA->>WatchA: Double-tap, capture source, allow 2s cancellation
    WatchA->>PhoneA: AppMessage 3 - request counter and source bundle
    PhoneA->>PhoenixAPI: POST /v1/drops - stable UUID and optional source
    PhoenixAPI->>PostgreSQL: Lock installation, check duplicate, limits and echo context
    PostgreSQL-->>PhoenixAPI: Commit drop or return original receipt
    PhoenixAPI-->>PhoneA: Accepted kind and request UUID
    PhoneA-->>WatchA: AppMessage 5 - accepted or reconciled without tick
    PhoenixAPI->>PhoenixAPI: OTP timer every 60s, also finalise before polls
    PhoenixAPI->>PostgreSQL: Group completed windows, insert immutable bundles
    PostgreSQL-->>PhoenixAPI: One bundle per non-empty minute
    PhoneB->>PhoenixAPI: GET /v1/pond?after=cursor - about once a minute
    PhoenixAPI->>PostgreSQL: Read newest live bundle and compute recipient flags
    PhoenixAPI->>PostgreSQL: Record issued_bundles eligibility
    PostgreSQL-->>PhoenixAPI: Bundle, has_other and echo_answer
    PhoenixAPI-->>PhoneB: Bundle and installation-scoped cursor
    PhoneB->>WatchB: AppMessage 2 - state and snapshot flag
    WatchB->>WatchB: Update visual state and deduplicate bundle
    WatchB->>WatchB: Fresh live event only - check local comfort policy
    WatchB->>WatchB: Persist budget before any eligible incoming vibration
    WatchB->>PhoneB: A later deliberate drop may reference this bundle
    PhoneB->>PhoenixAPI: POST /v1/drops - validate as echo or accept plain drop
```

The installation/request UUID pair is the idempotency key. Retries reconcile the original receipt without creating another drop or replaying an acceptance tick. Acceptance checks the 10-second cooldown and 100 contributions per rolling 24 hours. An invalid or expired echo context is downgraded to a plain drop.

The aggregator finalises completed minute windows every 60 seconds and before polling. Advisory locks coordinate acceptance with finalisation; immutable bundle inserts tolerate repeated finalisation. Polling returns only the newest unseen live bundle, with an installation-scoped signed cursor. Issued context enables a later echo; recipient flags are computed from accepted contributions.

Incoming haptics require a fresh live event and the watch's local policy: Silent by default; Gentle allows at most four in rolling 24 hours, separated by at least 90 minutes, respecting quiet hours, pause, available system quiet state and trustworthy time. The watch reserves its persistent budget before vibrating.

## PostgreSQL model

```mermaid
erDiagram
    direction LR
    installations["installations"] {
      uuid id PK "Opaque identity"
      bytea credential_hash UK "SHA-256 only"
      bigint last_seen "Expires after 30 days inactive"
    }
    drops["drops"] {
      uuid installation_id PK, FK "Cascade on installation delete"
      uuid request_id PK "Composite key makes retries idempotent"
      bigint accepted_at "Rate limits and 24-hour retention"
      bigint window_id "Logical bundle ID, no foreign key"
      bigint source_bundle "Nullable echo source, no foreign key"
    }
    bundles["bundles"] {
      bigint id PK "Minute ID, retained up to 24 hours"
      integer drops "Immutable accepted contribution count"
      integer echoes "Immutable echo count"
    }
    issuedBundles["issued_bundles"] {
      uuid installation_id PK, FK "Cascade on installation delete"
      bigint bundle_id PK "Logical bundle ID, no foreign key"
      bigint issued_at "Eligibility record, retained 24 hours"
    }
    deletions["deletions"] {
      uuid id PK "Revoked identity, deliberately no FK"
      bigint deleted_at "Eight-day tombstone"
    }
    installations ||--o{ drops : "owns accepted contributions"
    installations ||--o{ issuedBundles : "receives eligible context"
    bundles o|..o{ drops : "window_id logical match"
    bundles o|..o{ issuedBundles : "bundle_id logical match"
```

The two solid relationships are actual foreign keys with cascade deletion. Dotted relationships are logical matches, not database constraints: `drops.window_id`, `drops.source_bundle` and `issued_bundles.bundle_id` refer to minute IDs without foreign keys. A drop exists before its minute is finalised; anonymous bundle totals can outlive deleted contributors. `source_bundle` is a second logical link to a prior bundle, represented by the field rather than a duplicate edge.

`deletions.id` deliberately has no foreign key: revocation must survive deletion or an uncertain bootstrap. Leaving inserts a tombstone and removes the installation with its drops and issued context. Late registration using that identity is rejected while the tombstone remains. Aggregate bundles are not rewritten.

| State | Lifetime |
| --- | --- |
| Raw drops and issued context | 24 hours |
| Aggregate bundles | 24-hour retention; live for 2 minutes after their minute closes |
| Installation | 30 days without activity |
| Deletion tombstone | 8 days |
| Watch preferences/avatar/haptic budget | Local only; never in this database |

Reads enforce expiry; startup/hourly cleanup removes expired rows, so physical removal can lag the cutoff by one cleanup interval.

## Implementation references

- [HTTP and AppMessage contracts](PROTOCOL.md)
- [Pond context and transactions](../server/lib/pond/pond.ex)
- [OTP aggregation and cleanup](../server/lib/pond/aggregator.ex)
- [Database migration](../server/priv/repo/migrations/20260907000000_create_pond.exs) and [window_id rename](../server/priv/repo/migrations/20260907000001_rename_window.exs)
- [Implementation status](IMPLEMENTATION.md)
