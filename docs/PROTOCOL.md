# Pond protocol v1

The HTTP API lives in `server/`. All authenticated requests, including bootstrap and deletion retries, use `Authorization: Bearer <credential>`. Credentials are 32 cryptographically random bytes encoded as 64 lowercase hexadecimal characters. Generate them in the settings browser using `crypto.getRandomValues`, before registration, and persist them on the phone before sending. A registration retry therefore has the same identity even if its first response was lost. The server stores a SHA-256 hash and a derived opaque installation ID, never the credential.

Use HTTPS in real installations. The only HTTP exception is a loopback development origin. Origins contain a scheme, host and optional port, with no path, user information, query, or fragment. Never configure credentialed endpoints to redirect. XHR sends credentials only in Authorization, checks the final response origin when available, and relies on the phone runtime stripping Authorization across an origin change. Verify that behavior on supported mobile runtimes before publication.

Origins are canonicalised before persistence and comparison: scheme/host casing and explicit default ports do not create different identities. Existing saved origins are normalised without replacing their credentials. Actual cross-origin responses are still rejected.

Settings open on the selected service. For a custom service, the bridge first makes an unauthenticated `HEAD /settings.html` probe, serialised with other HTTP work. On failure or timeout it opens the original build's settings host as a labelled recovery page. This does not change the selected API origin or put its credential in the configuration URL. Local comfort changes still work; deletion continues against the selected API and stays pending until acknowledged. If both settings hosts are unavailable, switch faces for immediate quiet; restoring a settings host or rebuilding with a working settings origin is required to reopen the page. Recovery does not erase a pending-deletion credential.

## HTTP

| Method/path | Request | Response |
|---|---|---|
| `POST /v1/installations` | `{"version":1}` | `{version,id}`; bootstrap retries return the same ID |
| `POST /v1/drops` | `{version:1,request_id:<UUID>,source_bundle:<integer or null>}` | `{version,request_id,accepted_at,bundle_id,kind,duplicate}` |
| `GET /v1/pond?after=<cursor>` | No body; omit cursor for a silent snapshot | `{version,server_time,cursor,bundle,latest_minute_active}` |
| `DELETE /v1/installation` | No body | `{version:1,deleted:true}`; repeated deletion is acknowledged |
| `GET /health` | No credential | `{version:1,status:"ok"}`; process liveness |

All times are Unix seconds. Bundle IDs are `floor(accepted_at / 60)`. A bundle contains `id`, `starts_at`, `ends_at`, `expires_at`, `drops`, `echoes`, `has_other`, and `echo_answer`. These last two flags are computed for the authenticated installation without exposing contributors. Expiry is two minutes after the minute ends. Counts belong to one fixed window. A response contains at most the newest unseen live bundle. A cursor is signed, installation-scoped, and valid for 24 hours; a rejected cursor triggers a new silent snapshot.

An echo is accepted only against an unexpired bundle issued to this installation that still contains another installation's contribution. Otherwise the same action is accepted as a plain drop. The actual classification is returned. Accepted contributions (drops and echoes together) must be at least 10 seconds apart, with a ceiling of 100 per rolling 24 hours per installation. Same-ID retries reconcile the existing receipt before those limits are checked. The ID remains unchanged on retries, which stop after two minutes on the bridge. A receipt means persistence, not delivery to another wearer.

Errors use `{version:1,error:<code>}`: 401 for missing/expired credentials, 410 for a revoked bootstrap, 422 for invalid requests/cursors, 429 for rate limits, and 503 for installation capacity. Ingress throttling and parser errors may use a minimal error body. Clients treat all errors visually and quietly. The request body limit is 4 KiB.

## AppMessage

`watch/package.json` declares named keys; the SDK assigns their numeric IDs. Both sides use those generated mappings. Do not assume zero-based wire keys.

| Type | Direction | Purpose |
|---|---|---|
| 1 | Phone → watch | Complete settings with monotonically increasing `Revision` |
| 2 | Phone → watch | `ServerTime`, `Snapshot`, `Bundle`, `Expires`, `Other`, `Echo` |
| 3 | Watch → phone | `Request` counter and captured `Source` bundle |
| 4 | Watch → phone | `Applied` revision, resolved `PauseUntil`, current `Preview` eligibility |
| 5 | Phone → watch | `Request` and `Result`: 0 retrying, 1 first acceptance, 2 reconciled acceptance without tick, 3 unresolved terminal outcome, 4 rate limited, 5 rejected |
| 6 | Phone → watch | One immediate preview attempt; never retried |

The normal state dictionary is eight 32-bit values: 89 bytes including dictionary/tuple overhead, below the 128-byte target. Settings use a larger 256-byte inbox; the outbox is 128 bytes. The phone serializes AppMessage sends and HTTP requests separately, with bounded message queuing.

The watch retries a drop transport failure at most eight times, one second between attempts, retaining the same request counter and captured source. Immediate and asynchronous failures display Unknown. Transport retries stop on delivery acknowledgement, a matching bridge receipt, disconnect, leave, or the original watch timeout. They do not create an offline queue or extend the visual submission deadline. Once the phone receives the request, its two-minute same-UUID HTTP retry contract applies. Credential expiry stops authenticated activity but retains the local settings reconciliation loop until participation-off is acknowledged.

Since watch 0.1.1, an HTTP retry receipt keeps the watch showing Sending without extending its original deadline. A definitive rate limit shows Rate Limit; another definitive rejection shows Not Sent. A rejection following an uncertain HTTP attempt remains Unknown because a previous attempt could have been accepted. Terminal receipts are persisted on the phone for two minutes and retried up to three times after AppMessage failure; acceptance retries are silent. A matching duplicate watch request or bridge restart can replay that receipt without creating another HTTP drop. Leaving or starting a new watch request invalidates the cached receipt.

Pause commands `-3` (one hour) and `-2` (tomorrow at 08:00) resolve on the watch when a new revision is applied. `-1` is indefinite; `0` resumes usual settings. A retry of an already-applied revision keeps the same deadline. The acknowledgement returns the actual watch-local deadline to the phone.

Settings status is inspected by reopening the Pebble settings page after saving. The page cannot receive a live acknowledgement after its webview has closed. Watch acknowledgement and server deletion are tracked independently. A participation-off acknowledgement does not imply server deletion, and vice versa.
