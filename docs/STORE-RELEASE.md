# Pebble Store release

## 0.1.1 — 2026-09-07

Published an update after a Time 2 wearer reported that hard taps were required and a drop showed Unknown. The accelerometer threshold is now 900 mg instead of 1,800 mg, retaining the pairing and motor-settling guards and 50 Hz sampling. Terminal phone-to-watch receipts are cached and retried quietly; rate limits and definitive rejections have separate status labels. HTTP retries retain the original watch deadline and show Sending.

Validation: 17 bridge tests, the gesture/comfort/outbox native suites, and all seven SDK builds passed. Both the public Time 2 and Round 2 views showed version 0.1.1. The public PBW matched the local tested bundle byte for byte: SHA-256 `ae7f77919fb440bfedd78b061d7310be6c092658deb84b3eef82ece121db5e90`.

The original Unknown could not be attributed to a specific failure from available evidence. Physical comfort, accidental-trigger rates and delivery confirmation on the wearer's device still need verification. No server changes were required.

## 0.1.0 — 2026-09-07

Pond 0.1.0 was published on 2026-09-07 through the Pebble Developer Dashboard, using the signed-in Brave browser session.

- Public listing: https://apps.repebble.com/9101f10632be4625b5db4655
- App ID: `9101f10632be4625b5db4655`
- Type: watchface. Public developer credit: `monocursive`.
- Bundle: `watch/build/watch.pbw`, rebuilt with `Monocursive` as the package author and `https://pond.monocursive.com` as the default service origin.
- SHA-256: `e3f61e4a4050f3164f94edf8e1210d2cf786a0908b9884d7e7526b6fd7054b5b`.
- All seven platforms are included: aplite, basalt, chalk, diorite, emery, flint and gabbro.
- Uploaded two native screenshots and one matching banner per platform, plus the 80px and 144px icons from `design/store/`.

The dashboard reported both app and release as Published. The public Time 2 and Round 2 views showed Pond, the correct developer credit, version 0.1.0 and two screenshots. The Round 2 banner was visually checked. Website and source links were verified after saving; the public downloadable PBW matched the local production bundle byte for byte. The production readiness endpoint returned ready.

The listing and release notes explicitly describe an early release. Publication does not establish physical watch/phone compatibility, gesture reliability, haptic comfort or battery performance. Those tests and the remaining operational work in [IMPLEMENTATION.md](IMPLEMENTATION.md) are still outstanding.
