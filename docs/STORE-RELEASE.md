# Pebble Store release

Current gesture guidance: try two short, quick wrist twists (a back-and-forth motion), then repeat within two seconds to cancel. Based on Time 2 feedback, this feels more like a wrist twist than a screen tap. Pond uses accelerometer motion, not touchscreen input. Recognition remains experimental; historical release notes below describe earlier terminology.

## 0.1.4 — 2026-09-07

Rebuild of the 0.1.3 haptic fix from committed source, with the wrist-motion documentation and hosted settings guidance tracked alongside it. Watch behaviour is unchanged from 0.1.3. The version advances because 0.1.3 was already published.

## 0.1.3 — 2026-09-07

Published the haptic-hold fix for all seven platforms. Preview eligibility and motor playback now share a tested policy gate: the incoming 24-hour hold and rolling allowance no longer suppress explicit previews or opt-in accepted-drop ticks. Gentle mode, trusted time, quiet hours, pause and system Quiet Time still apply. Incoming haptic reservations and silent replay handling are unchanged.

Validation: native policy regressions, 17 bridge tests, two landing/settings tests, and all seven SDK builds passed. Bundle SHA-256: `df07c0c6a8f22d0904f052456ef992a0e00a8509f721467deb431d262b49c6e7`. The developer dashboard confirmed 0.1.3 as Published; the public Time 2 page still served the 0.1.2 download at verification time. Physical haptic confirmation remains outstanding.

The settings explanation and preview availability hint are live in Kamal deployment `pond-20260907-haptics-copy`; settings HTML/JavaScript and readiness returned HTTP 200. This release was initially built before its source changes were committed; 0.1.4 is the rebuild from committed source.

## Gesture wording update — 2026-09-07

Updated the published listing description and 0.1.2 release notes to explain wrist twists, accelerometer input, and repeating the gesture within two seconds to cancel. Reopening the dashboard confirmed both edits persisted. The public Time 2 page still displayed the previous description after refreshing at verification time; public propagation was not yet confirmed.

The matching settings and landing-page copy is live in Kamal deployment `pond-20260907-gesture-copy`. Both pages returned HTTP 200 with the new wording, `/ready` returned ready, and the two existing landing/settings tests passed. This is a documentation update: the 0.1.2 watch binary and gesture detector are unchanged. Reopen settings in the Pebble phone app to load the new instructions.

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
