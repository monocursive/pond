# Pond — First release specification

**Version:** 0.1, draft · **Date:** 2026-09-07 · **Owner:** Michaël / Monocursive

**Release:** first public release, preceded by a private comfort beta.

**Status:** implementation contract. A first local prototype now exists; see [implementation status](IMPLEMENTATION.md) for evidence and remaining work. Physical validation, deployment, and publication are not yet complete. Numeric comfort and battery thresholds below are proposed engineering/playtest gates, not established physiological limits.

**Related:** [North star](NORTH-STAR.md) · [Original proposal](archive/PRODUCT-SPEC-v0.2.md)

## 1. Promise and scope

Pond is a readable Pebble watchface with a little generated stone. Join one shared pond, drop your pebble, and occasionally see or feel that other people are there. You can answer a shared ripple, or leave it alone.

The first release proves three things: the gesture is deliberate, incoming haptics remain welcome, and a small community can maintain the service. Elixir / Phoenix is the chosen backend.

| Included | Deferred to the north star |
|---|---|
| One C watchface and PebbleKit JS bridge | Separate watchapp and on-watch maker |
| Generated local avatar, optional preset customisation | Visiting avatars, personal history, reach counts |
| One global pond; drop, ripple, bundle-level echo | Location, shallows, compass |
| Silent mode, gentle mode, quiet hours, reduced motion | Health permissions and sleep detection |
| Explicit join/leave, rate limits, bounded retention | Resonance, direct contact, heart rate |
| Open source, reproducible builds, self-hosting | Rain, timeline announcements, sound, RGB effects |

No accounts, user-authored messages, image uploads, advertising, monetisation, or engagement analytics. A published protocol is included for maintenance and interoperability; a separately supported developer API programme is out of scope.

## 2. First use and settings

The installed face immediately shows local time and a generated pebble. It works before the network is available. Initial state is **not joined, silent**, with no backend registration or activity polling until the wearer joins.

The existing Pebble mobile app's settings page provides a short explanation and a **Join the pond** action. There is no separate phone app or sign-in. After joining, participation remains silent until gentle haptics are explicitly enabled. This small setup step replaces the original promise of an automatic on-watch maker.

Settings use Clay where practical and include:

| Setting | Initial value / behaviour |
|---|---|
| Participation | Not joined; explicit join |
| Incoming haptics | Silent; optional Gentle |
| Accepted-drop feedback | Visual only; optional tick in Gentle mode |
| Quiet hours | 22:00–08:00 watch-local time; editable, including an explicit all-day quiet option |
| Pause incoming haptics | One hour, until tomorrow at 08:00 local, or indefinitely |
| Motion | Brief ripple animation; Reduce motion replaces it with a static state |
| My pebble | Optional curated silhouette/palette presets and shuffle; never a requirement |
| Service | Default community endpoint; advanced HTTPS endpoint setting |
| Leave the pond | Stop participation and request server deletion |

Saving settings reports **Applied on watch** only after a watch acknowledgement. When disconnected, show **Waiting for watch**; do not claim the watch has muted. Switching away from the Pond face is an immediate way to stop its watch-side activity. No background watch worker is included.

A haptic preview is only available in Gentle mode outside quiet hours. It is explicitly triggered, sent once, never queued across disconnection, and limited to once per ten seconds. The page explains which settings affect visuals, vibration, or participation.

## 3. The watchface

Large, high-contrast time stays visible in every state. The pebble is still at rest. Use silhouettes and contrast, not colour alone, for state meaning. Support system 12/24-hour time preferences and layouts appropriate to rectangular and round screens.

Below the time, a small status area has room for a pond state and a recent ripple marker. No total user count, daily contribution target, notification badge, or scrolling history.

| State | Presentation |
|---|---|
| Not joined | Pebble and a small setup indication; no recurring prompt |
| Connected, latest completed minute had no drops | Calm pond; no invented activity |
| Fresh activity | One ring animation, at most 1.5 seconds, then a static recent-ripple marker |
| Reduced motion | Static recent-ripple marker from the outset |
| Preparing a drop | Small visual change during the cancellation interval |
| Accepted | Brief visual confirmation; optional tick under the comfort policy |
| Outcome unknown | Neutral pending/unknown marker; no success claim |
| Offline or activity state older than two minutes | Disconnected/stale indicator, not an empty-pond claim |

Fresh silent activity may update the display at most once per minute. Haptic eligibility is evaluated separately. Do not flash the backlight, drift continuously, or animate the resting face. A recent-ripple marker expires after two minutes, independent of whether the wearer responds.

## 4. Gesture and contribution contract

### Proposed gesture

Two deliberate taps within 600 ms begin a drop. A second recognised double-tap during the following two seconds cancels it locally. There is no cancellation vibration. Only send after the local cancellation interval; the service does not need a two-second holding queue.

This timing is a prototype value. Magnitude gating requires acceleration samples because the platform tap callback exposes only axis and direction. Sampling, dominant-axis assumptions, and battery cost must be validated across supported devices. Reject samples contaminated by vibration and suppress gesture recognition during Pond haptics plus a measured settling interval.

No long-shake mute gesture ships in this release: it would add another motion classifier and potential surprise. Settings provide pause/silent controls; switching faces stops Pond immediately.

### Drop or echo

At the first recognised double-tap, capture the most recent live bundle presented on this watch, if it is no more than two minutes old and contains a contribution by another installation. This includes a ripple presented silently. If eligible, the contribution is an echo of that bundle; otherwise it is a plain drop.

There is no countdown or reward for speed. The two-minute limit prevents replying to stale context. It does not restrict when a wearer may make a new drop.

Echoes address a bundle. They do not choose a featured avatar or form a relationship. If the bundle expires before submission is accepted, accept the action as a plain drop and return its actual classification.

### Network and limits

- Generate a unique request ID before submission. Duplicate IDs return the original result without another contribution or confirmation tick.
- Allow at most one accepted contribution per installation per 10 seconds, and 100 in a rolling 24 hours. Echoes count toward both limits. Limits are ceilings, never displayed goals.
- A deliberate submission can be retried with the same ID for up to two minutes. After that, stop resending. No durable offline outbox of old drops.
- A timed-out request may already have been accepted. Show an unknown result until reconciled; never substitute a fresh ID automatically.
- Show “accepted” only after a server response confirms persistence. It means the drop entered the pond, not that another watch received it.
- A server-rejected or rate-limited action uses visual feedback only. Extra taps do not produce repeated error buzzes.

## 5. Comfort policy — watch is final authority

The watch evaluates current local policy immediately before playing any pattern. Server messages describe events, never instructions that bypass policy.

| Rule | First-release contract |
|---|---|
| Initial mode | Silent |
| Gentle incoming allowance | At most four unsolicited presentations in a rolling 24 hours |
| Spacing | At least 90 minutes between any two unsolicited presentations |
| Quiet hours | Default 22:00–08:00 local; overrides incoming, confirmations, and previews |
| Pause | Suppresses incoming haptics immediately once applied on watch; no backlog on expiry |
| System quiet mode | Suppress all Pond haptics when detectable; capability and behaviour must be tested for each supported firmware |
| Missing policy, budget, or reliable time | Suppress haptics until state is trustworthy; initialise an unknown incoming budget with a conservative 24-hour hold |
| Restart, reconnect, face relaunch | Preserve budget; initialise current state silently; never replay pending haptics |
| Echo answers | Share the four-presentation allowance; never create an additional interrupt channel |
| Accepted-drop tick | Opt-in, only in Gentle mode, once per newly accepted request and within submission limits; no delayed tick after a reconnect |

The rolling budget is stored on the watch. A timezone change affects quiet hours but cannot replenish the elapsed-time allowance. Backward clock changes or lost budget state must not increase it. Persist the budget reservation before triggering an unsolicited haptic; a crash may conservatively consume an unused slot, never restore one that could already have played.

A poll can refresh visual state even when no incoming haptic is allowed. Suppressed events are discarded for haptic purposes. There is no morning summary, “you missed” event, or catch-up buzz. A server outage, registration issue, or low activity level never increases the allowance.

### Initial haptic vocabulary

Prototype with on/off timing, tuned on real motors. No amplitude ramps are assumed.

| Meaning | Candidate timing | Eligibility |
|---|---|---|
| Your drop was accepted | One 60 ms pulse | Optional direct feedback |
| A new shared ripple arrived | 80 ms on, 120 ms off, 80 ms on | One incoming slot |
| A shared ripple you contributed to received an echo | Three 60 ms pulses separated by 100 ms gaps | Replaces the ordinary pattern in one incoming slot |

When an eligible fresh bundle includes an echo to a bundle containing one of your accepted drops, use the echo-answer pattern instead of the ordinary pattern. Never play both. Without an eligible slot, update the small visual state only; do not retain a haptic debt. Copy says **“Your shared ripple received an echo”**, never “someone chose you.”

## 6. Aggregation and delivery

Use one global pond and fixed, non-overlapping 60-second windows based on server acceptance time. Each accepted contribution belongs to one window. Finalise one immutable bundle per non-empty window; do not repeatedly recount a drop in overlapping rolling windows.

Bundles contain a stable ID, window timestamps, expiry, and aggregate drop/echo counts. There are no featured visitor seeds, public contributor identifiers, location fields, or individual reply assignments. The recipient response may include a boolean indicating a truthful echo to a bundle they contributed to.

The watch's own contributions do not trigger incoming haptics or make a ripple eligible to echo when nobody else contributed. Compute receiver eligibility without exposing the contributor list. Server-side echo validation rejects self-only context and references never issued to that installation; a bundle ID is not an authorisation token.

PebbleKit JS polls using HTTPS approximately once per minute while its runtime is active, with jitter, a cursor, bounded timeouts, and exponential backoff on failure. It does not run overlapping requests. On a fresh connection or face launch, fetch current state as a **silent snapshot**. A subsequent poll may present only the newest unseen, unexpired bundle. Older bundles are skipped for presentation; missed history is not replayed.

A bundle expires for live presentation two minutes after its window closes. Client presentation of the same ID is deduplicated, including after restarts. Background phone execution is an explicit release experiment: HTTP polling is a transport choice, not a guarantee against runtime suspension.

No delivery or reach count is exposed in this release. Test instrumentation may verify watch receipts, but transport success must not be described as proof that a person felt anything.

## 7. Architecture and protocol boundaries

### Watch and phone

- **C watchface:** time, avatar, drawing, gesture classifier, persistent settings/budget, haptic policy, compact AppMessage payloads.
- **PebbleKit JS:** configuration, installation credential, HTTPS requests, retry/cursor logic, and conversion into watch messages. Keep the bridge compatible with the actual supported mobile runtimes.
- **Rendering:** use simple paths/primitives or PDC only where it helps. Explicit monochrome rendering; no assumption that PDC automatically dithers colours.
- **Payload target:** at most 128 bytes for a normal state message including AppMessage dictionary overhead; verify actual buffers on every target. This is a target to measure, not a claim that the JSON protocol fits in 128 bytes.
- **Shared state:** only the face and its own bridge. There is no cross-app persistent storage or identity sharing in this release.

### Elixir service

Phoenix HTTP endpoints, Ecto, PostgreSQL, and one global aggregation process under an OTP supervisor. One application instance initially; PostgreSQL holds accepted events, rate-limit state, idempotency records, and bundle finalisation state. No Channels, Presence, Redis, clustering, or regional aggregators required.

Use transactions and uniqueness constraints to enforce contribution limits and idempotency under concurrent requests. The aggregator finalises windows transactionally and resumes from database state after a crash. Recovery may rebuild recent valid state but never instruct clients to replay expired haptics. Supervision alone is not a delivery guarantee.

### Proposed endpoints

| Endpoint | Purpose |
|---|---|
| `POST /v1/installations` | Register after join; return a random identity and revocable credential |
| `POST /v1/drops` | Idempotent contribution with request ID and optional source bundle ID |
| `GET /v1/pond?after=...` | Current state and newest eligible bundle, plus opaque next cursor |
| `DELETE /v1/installation` | Revoke current installation and delete its linked data |
| `GET /health` | Minimal operational health, no participant data |

Registration must have an idempotent bootstrap strategy so an uncertain response does not multiply identities. Store only credential hashes server-side. Validate schema/version, body sizes, cursor ownership, and echoed-bundle eligibility. The server derives identity from credentials; never trust a client-supplied avatar seed as identity.

Installation credentials are scoped to a server origin. Changing endpoint requires joining that server with new credentials; never forward credentials from the previous host, including through cross-origin redirects. Explain that joining another host does not delete the old installation.

## 8. Privacy and abuse limits

Do not request Health or location permissions. Do not use the account token. Generate the avatar seed separately from credentials; sharing or shuffling an avatar never grants access or resets limits.

An installation is pseudonymous, not proof of one person or one physical wrist. Bound registration and per-credential traffic, add short-lived coarse abuse throttles at the edge, and cap resources globally. Normal app request logs omit credentials, request bodies, persistent installation identifiers, and IP addresses. Any hosting-layer logs must follow the documented policy too.

Fixed avatar presets and no visitor display remove the first release's expressive moderation surface. Individual blocking/reporting and a social graph are not required because there is no direct contact or selectable stranger. Operators still need to revoke abusive credentials and protect availability. No text does not mean no abuse.

| Data | Retention and purpose |
|---|---|
| Raw drops, source-bundle references, issued-bundle eligibility, idempotency/rate-limit records | Up to 24 hours for aggregation, echo validation, retries, and rolling limits; expired data is never queried as live activity |
| Aggregate bundles with no contributor identifiers | Up to 24 hours for recovery; live presentation expires after two minutes |
| Installation ID, credential hash, last-contact time | Until explicit leave or 30 days without authenticated contact; no long-term activity history |
| Avatar and comfort preferences | Watch / bridge storage only; the server needs neither the avatar nor quiet-hour schedule |
| Transient abuse-throttle keys | At most 24 hours; no reuse for analytics |
| Sanitised operational logs | Seven days; no participant payloads or persistent participant identifiers |
| Encrypted database backups | Seven days, then expiry; restoration reapplies deletions before serving traffic |
| Minimal deletion tombstones | Eight days to cover backup restoration, containing only opaque revoked installation IDs; purge afterwards |

Expiry checks apply on reads immediately; cleanup runs at least hourly, so physical removal can lag the stated lifetime by at most one hour. Already delivered anonymous aggregates cannot be withdrawn from watches and may expire naturally. The published privacy notice must state this limit.

**Leave:** stop future bridge requests immediately except deletion/reconciliation, request a participation-off update on the watch, revoke the server credential, and delete linked live records. Display watch-state and server-deletion acknowledgements separately. When offline, retain only the credential needed to retry deletion and show it as pending. Do not erase that credential before successful deletion or an explicit local-only reset. Uninstalling alone does not notify the service; inactive registrations expire after 30 days.

## 9. Supported devices and graceful failure

Target all seven C SDK platforms: Aplite, Basalt, Chalk, Diorite, Flint, Emery, and Gabbro. Use current SDK capability checks and budgets. Keep Aplite's 24 KiB code-plus-heap constraint and monochrome readability in the earliest prototype.

Build/emulator coverage and physical support are separate entries in a release matrix. Before the first public release, demonstrate the full loop on at least one legacy monochrome model and one newer model, across iOS and Android phone pairings. Public compatibility claims list the exact tested watch, firmware, phone OS, and mobile app combinations. Other compiled targets remain experimental until tested; broad support remains the north-star goal.

Offline operation retains local time and avatar. Gesture transmission failures are honest and quiet. If the service closes permanently, those local functions continue. An expired installation asks the wearer to rejoin through settings; it never silently opts them back in or restores haptics.

## 10. Validation and stop conditions

### First experiment: two watches

Two real watches and two phones exchange one drop, a shared ripple, and one echo. Repeat with phones locked, the mobile app backgrounded, temporary Bluetooth loss, and face switching. Record measured latency and runtime availability; do not extrapolate from an emulator.

If phone background limits prevent the promised ambient behaviour, pause public release and revise delivery or the product promise. Adding a WebSocket does not by itself pass this gate.

### Private beta

Invite approximately 10–20 volunteers for at least a week, with no participation quota. Include silent wearers and people who opt into haptics. Ask about comfort, confusion, time readability, accidental contributions, and battery effects. Muting, reduced motion, or deciding to leave is useful feedback, not a negative performance metric.

Collect feedback voluntarily; no automatic user-journey analytics. Bounded diagnostic capture is opt-in, expires after seven days, and excludes credentials and sensitive phone data. Results are reported as test observations, with sample size and devices.

### Required gates

| Area | Acceptance evidence |
|---|---|
| Haptic policy | Deterministic checks cover rolling limits, spacing, quiet hours, pause, duplicate bundles, restart, reconnect, missing state, and clock changes; zero policy bypasses |
| Motion safety | Zero self-generated drops from vibration in targeted tests; at least 95% recognition across 100 deliberate attempts per physically supported model; no more than one unintended accepted drop per seven wrist-days across at least 30 beta wrist-days |
| Delivery | At least 95% of 30 eligible test contributions appear within 120 seconds across the declared background/locked-phone test matrix, when Bluetooth/network remain available; report all failures and conditions |
| No replay | Reconnect and server restart tests produce no backlog of haptics; expired drops are not resubmitted |
| Correctness | Concurrent duplicates and retries yield one accepted contribution; expired/self-only/unauthorised echoes cannot create personal feedback |
| User control | Join, silent mode, quiet hours, pause, and leave have understandable states; disconnected saves/deletion never claim completion |
| Accessibility | Time is legible on all target layouts; monochrome and reduced-motion states carry the same meaning; no critical information requires haptics |
| Battery | Paired 48-hour baseline/Pond runs on representative legacy and newer hardware; proposed ceiling of five extra percentage points of battery capacity consumed per 24 hours on legacy hardware and three on newer hardware, under matched conditions |
| Operations | Restore a backup, reapply deletion tombstones, restart aggregation, revoke a credential, and verify bounded cleanup without resurrecting live activity |
| Community handoff | A second person builds the watchface and runs the service from documented instructions without private maintainer knowledge |

Battery readings are coarse: document charging, usage, hardware condition, and uncertainty, and repeat inconclusive comparisons. Do not claim a battery percentage from simulator estimates. A comfort or motion failure blocks shipping haptics/gesture support for the affected model until fixed or explicitly redesigned; it must not be concealed by lowering a threshold after testing.

## 11. Open-source release and deployment

Prepare the watchface, bridge, and backend source with a proposed MIT licence, a brief contribution guide, third-party notices, and a code of conduct. Verify rights to all included assets and dependencies before applying the release licence. Licence choice remains a drafting proposal, not a claim that existing files are already licensed.

Provide pinned toolchain versions, lockfiles, build instructions, `.pbw` artifacts with checksums, a container/Elixir release, database migrations, environment-variable examples without secrets, and a two-service application/PostgreSQL deployment example. Document TLS, backups, deletion restoration, upgrade/rollback constraints, and server endpoint replacement.

Host one community pond initially. Record measured resource needs and an affordable operating budget after load testing; do not promise a price or “a few hundred lines” in advance. Prepare a simple landing page, privacy notice, support route, and store listing with exact tested compatibility and an honest description of optional haptics and phone connectivity.

Publish only after live staging validation and the gates above. Launch can be a friendly community invitation; it does not depend on Rain or manufactured activity. No store submission, external announcement, production deployment, or licence application is performed merely by approving this spec.

## 12. Done and deliberately open

Done means the bounded face/bridge/service works on the declared supported combinations, comfort and reliability gates have evidence, the service is deployed, installable artifacts and source are public, and another person can operate their own pond. Keep local build/test proof, physical beta proof, and live release proof distinct in the release checklist.

Implementation decisions left to experiments: exact gesture thresholds and vibration timing; lowest-cost reliable polling behaviour; verified firmware quiet-mode handling; final hardware support matrix. These are release gates, not invitations to add the deferred feature set.

The first release does not need 1,000 users, daily drops, an echo conversion target, or resonance pairs to succeed. A few people who enjoy keeping Pond on their wrists are enough.

## Sources and precedence

Platform constraints are grounded in the current developer documentation linked in [the north star](NORTH-STAR.md#documentation-references). Comfort limits and interaction choices are product proposals developed from the initial spec and discussion, not platform guarantees.

If the original draft, north star, and this document differ, this document governs the first release. Changes to this scope should update its acceptance criteria before implementation.
