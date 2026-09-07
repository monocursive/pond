# Pond — Product Spec

**Version** 0.2 (draft) · **Date** 2026-09-07 · **Owner** Michaël / Monocursive · **Status** for discussion

*Changes in 0.2: name confirmed as Pond; "Tide" renamed "Rain" throughout; new §2.1 on naming; rain haptic pattern and face treatment revised.*

---

## 1. One-liner

Pond is a Pebble watchface where every wearer is a low-poly pebble. Drop yours, and everyone else wearing a Pebble feels the ripple on their wrist.

## 2. Intent

**The feeling we are after.** "I am not the only one wearing this odd little watch right now." Ambient co-presence — the wrist-worn equivalent of hearing a distant bell and knowing someone rang it. Pebble's return is a community story, and the watch itself has no community layer. Watchie-Talkie proves people want watch-to-watch contact; Pond is the version that asks nothing of you.

**The medium is vibration.** You *feel* the community; you never read it. No screen time, no text, no typing, no scrolling. A ripple arrives, you feel it, you glance if you want, you drop your own pebble or you don't. The whole interaction fits between two heartbeats.

**Tone.** Playful, gentle, slightly strange. A toy with a soul, not a network. The kind of thing you show someone by grabbing their wrist and saying "wait, feel this."

**What Pond is not.**

- Not a messenger, feed, or social network. No text, no profiles beyond the pebble, no follower counts, no public activity log.
- Not an engagement machine. No streaks, no leaderboards, no "come back" notifications, no badges. The pond is there when you look at it and silent when you don't.
- Not a data business. Nothing is collected that the product does not need for the next sixty seconds.

### 2.1 The name

**Pond** is kept, and the metaphor is made consistent. Rationale:

- A pond is small enough that one pebble visibly disturbs it. That is principle 6 ("small numbers must feel special") in a word; an ocean-scale name would swallow a single drop.
- It is a place, not an action — matching "the pond is there when you look at it."
- It completes the Pebble metaphor without using the Pebble trademark, and no store app carries the name.
- The vocabulary is water at pond scale throughout: pebble, drop, ripple, echo, shallows, rain. Earlier drafts used "tide," which a pond does not have; the global event is now *rain*, and a drop is therefore both a pebble dropped and a raindrop.

The name is visual and the product is haptic. That gap is accepted: the vibration grammar (§7.5) carries the felt experience, and the name only has to get someone to install the face.

Considered and rejected: *Ripple* (three existing store apps and a cryptocurrency), *Murmur* and *Cairn* (both taken in the store), *Skip* (crowded by Skipstone and by "skip" meaning dismiss), *Swell* (right for a global-first product, wrong for one whose principles are intimate).

## 3. Goals and non-goals

### Goals (v1)

1. **Ambient co-presence** — a wearer feels other people several times a day without anyone asking for their attention.
2. **One-gesture joy** — a single physical gesture produces an immediate, physical response from strangers, and the wearer later learns how far it travelled.
3. **Zero friction** — no account, no phone companion app, no configuration page required. Install the face, make a pebble, you are in the pond.
4. **Every Pebble** — including black-and-white and pre-2025 watches served by Rebble. Excluding half the community defeats the point.
5. **Kind by construction** — spam, harassment, and doomscroll patterns are made impossible by the design rather than moderated after the fact.

### Non-goals (v1)

Messaging of any kind · monetisation · a phone app · a web presence beyond a landing page · a games layer · integration with other social platforms.

## 4. Design principles

1. **Feel first, see second.** Every feature must have a haptic expression. The screen is a caption, not the content.
2. **Aggregate, never fan out.** No wrist ever receives individual events from strangers. It receives the pond's state.
3. **Nothing to type, nothing to read.** If a feature needs text, it does not belong in Pond.
4. **Silence is a feature.** Quiet at night by default, muting is one gesture, and an empty pond is shown honestly rather than faked.
5. **The pebble is the identity.** One wrist, one pebble, no names. A pebble is recognisable by shape and colour alone.
6. **Small numbers must feel special.** One ripple from one person should feel better than a hundred, not worse. The product must work on launch day with thirty users.

## 5. Vocabulary

| Term | Meaning |
|---|---|
| **Pebble** | A wearer's avatar: a low-poly stone generated from a seed plus a few choices. Also the identity. |
| **Pond** | The shared space. One global pond; regional *shallows* used for aggregation and distance. |
| **Drop** | The act. A wearer double-taps the watch and their pebble enters the pond. Also a raindrop — see Rain. |
| **Ripple** | What everyone else feels: an aggregated wave of recent drops, delivered at most once a minute. |
| **Echo** | A drop made within ten seconds of feeling a ripple. It is credited back to that ripple. |
| **Reach** | How many wrists a drop's ripple arrived on. The wearer's only score, and it is private. |
| **Resonance** | Two pebbles that have echoed each other repeatedly. Unlocks direct ripples between them. |
| **Rain** | A scheduled global moment when everyone drops at once: rain on the pond. |

## 6. Core loop

```
wear the face
   │
   ▼
feel a ripple ──▶ glance (count · direction · one or two pebbles)
   │
   ├──▶ ignore                      (most of the time — that is fine)
   │
   └──▶ double-tap within 10 s ──▶ ECHO   ──▶ confirmation buzz
                                                 │
double-tap any other time ──────▶ DROP   ──▶ confirmation buzz
                                                 │
                                                 ▼
                                   later, in the app: reach + echoes
```

The loop has no obligation in it. The only "reward" is learning that a stranger felt you.

## 7. Experience

### 7.1 First run — the pebble maker

Runs entirely on the watch, with buttons. Three screens, thirty seconds:

1. **Shape** — Up/Down cycles through ~12 base silhouettes (rounded, flat, angular, cracked, egg, shard…). Select confirms.
2. **Colours** — three palette slots (body, facet, accent). On black-and-white watches these become three dither densities.
3. **Face** — Up/Down cycles a handful of triangle "expressions" (none, two dots, a notch, a crooked line). Select confirms.

Everything else — the exact triangle layout, the facet positions — comes from a seed derived from the watch token, so every pebble is unique without the wearer having to do more. A "Shuffle" long-press regenerates the seed. The pebble is shown tumbling once with a soft buzz, and the face installs. That is the whole onboarding.

Clay configuration on the phone exists only for advanced settings (see 7.4); the maker never requires it.

### 7.2 The watchface

Time, large, in a face that does not look like a novelty. Below it, three quiet elements:

- **Your pebble**, idle, drifting a pixel now and then.
- **Ripple count** for the last hour, small. Zero is displayed as zero.
- **A ring** around the pebble that expands and fades when a ripple arrives, with a bearing tick if the wearer has location on.

When a ripple arrives: the vibration pattern plays (7.5), the ring animates, and up to three featured pebbles tumble across the screen for ~1.5 s, then settle back to the idle face. Nothing persists on screen longer than that; the face returns to being a watch.

### 7.3 Gestures

Watchfaces cannot own the physical buttons (the OS reserves them), so all interaction on the face is by accelerometer tap:

| Gesture | On the face |
|---|---|
| Double-tap | Drop, or echo if within 10 s of a ripple |
| Double-tap again within 2 s | Cancel the drop (it is held server-side for 2 s before entering the pond) |
| Long shake (≈2 s) | Mute for one hour; one long fading buzz confirms |
| Quick Launch (system long-press, user-configured) | Opens the Pond app |

Tap thresholds are tuned to avoid bumps: two taps within 600 ms, above a magnitude gate, on the wrist's dominant axis. The 2-second cancel exists precisely because false positives will happen.

### 7.4 The Pond app

A conventional watchapp reachable by Quick Launch or the launcher. Menu:

- **Reach** — your last ten drops: how many wrists, how many echoes, the pebbles that echoed you.
- **Around** — a compass view: bearing and rough distance of the last ripple's centre of mass, if location is on.
- **Resonance** — pebbles you are resonant with; send a direct ripple; dissolve.
- **My pebble** — reopen the maker.
- **Settings** — quiet hours · location sharing (off by default) · heartbeat mode (Time 2) · sound (Time 2) · block list · report the last featured pebble.

### 7.5 Haptic grammar

The vibration vocabulary is the product. It must be learnable in a day and distinguishable through a sleeve.

| Event | Pattern | Character |
|---|---|---|
| Ripple, near (same shallow) | short · short | crisp, like a knock |
| Ripple, far | one long soft ramp (~400 ms) | a swell |
| Ripple, large (≥ 50 drops) | three rising pulses | a wave building |
| Your drop confirmed | one short tick | "sent" |
| Someone echoed you | tick · tick · tick, fast | pleased |
| Resonant direct ripple | lub-dub | a heartbeat |
| Rain | a patter of quick soft taps, building for ~2 s, then one swell | ceremonial |
| Mute engaged | one long fading buzz | "going quiet" |
| Morning ripple (what happened while you slept) | soft swell then two ticks | a summary |

On Pebble Time 2 the LRA motor allows sharper transients; on older ERM motors patterns degrade gracefully to the same on/off timing.

### 7.6 Pebble Time 2 extras (opt-in, off by default)

- **Sound** — a single short note through the speaker on a ripple; pitch falls with distance.
- **Glow** — the RGB backlight tints for a second in the ripple's dominant pebble colour.
- **Heartbeat** — thirty seconds of the wearer's live heart rate, sent as a resonant direct ripple or contributed to a rain's "pond heartbeat" (the median HR of participants). Never broadcast to strangers on its own.

## 8. Mechanics

### 8.1 Drops

- Soft limit: one drop per 30 s per wrist (extra taps are absorbed as the same drop). Hard limit: 40 per day.
- A drop is held for 2 s server-side to allow cancel, then enters the pond with: wearer's pebble seed, shallow id, timestamp, and `echo_of` if applicable.

### 8.2 Ripples

- The server aggregates drops per shallow in a rolling 60 s window and emits at most **one ripple bundle per wrist per minute**. A bundle contains: total count, near/far split, centre-of-mass bearing and distance bucket (only if the receiver shares location), up to three featured pebble seeds (weighted toward pebbles that have never been featured on this wrist), and a rain flag.
- Bundles are delivered only while the wearer is awake according to the Health API. While asleep they are accumulated into one **morning ripple**: "while you slept: 212 drops · 4 echoes of yours."
- If no drops occurred, no bundle is sent. Silence is real.

### 8.3 Echo and reach

- A double-tap within 10 s of a ripple is an echo, credited to the most recent bundle's contributing drops (pro-rata to the featured pebbles first, then randomly among the rest so every dropper has a chance of being echoed).
- Reach = number of wrists the bundle containing your drop was delivered to. Shown privately in the app, never publicly.

### 8.4 Resonance

- Two pebbles that echo each other three times within seven days become resonant. Both are told with the lub-dub pattern and shown the other's pebble.
- Resonant pebbles may send each other direct ripples, one per hour, and heartbeats on Time 2.
- Either side can dissolve resonance silently. Resonance expires after 30 days without a direct ripple.

### 8.5 Rain

- Scheduled server-side. Candidates: the New Year rain rolling across time zones; the Pebble Kickstarter anniversary (11 April); Core Devices milestones; a monthly rain at a fixed UTC hour, announced a day ahead as a timeline pin.
- During a rain, the bundle cadence tightens to every 15 s, the rain pattern plays, and the face changes: raindrops fall on the pond, every ripple ring overlaps, and a single global counter replaces the hourly one. Rain is the one moment large numbers are allowed to be the point.
- A rain lasts ten minutes. When it ends the face settles back and the wearer gets one summary buzz: how many pebbles were in the pond with them.

## 9. Privacy and safety

- **Identity** is `Pebble.getWatchToken()` on the phone side — opaque, per watch, never linked to a name, email, or account. The pebble seed derives from it one-way.
- **Location** is off by default. When on, the phone quantises it to a shallow (~50–100 km cell) before it leaves the phone. The server keeps it only for the current aggregation window. Precise coordinates never exist server-side.
- **No text anywhere**, so there is no language to moderate. The only expressive surface is a pebble with at most 24 triangles in a 64-colour palette, which limits what can be drawn.
- **Reporting**: from the app, report the last featured pebble. Three reports from distinct wrists hide a pebble from featuring pending review; the wearer keeps dropping (they contribute to counts) but is never shown. Blocking is per wrist and permanent.
- **Quiet hours**: automatic from Health sleep detection; manual window as fallback (default 22:00–08:00 local when Health is unavailable).
- **Retention**: raw drops are discarded after 24 h. What persists: per-wrist counters, resonance pairs, block lists, reports, the pebble definition. Deleting the face from the watch and tapping "leave the pond" in the app erases everything server-side.
- **Minors**: Pond is not designed for children. There is no chat and no precise location, which keeps the risk surface small, but the app is listed as 13+.

## 10. Platform approach

### 10.1 Watch side: C SDK

**Decision (proposed): C, not Alloy.** Rationale: the watch-side surface is small — draw a PDC avatar, play vibration patterns, subscribe to accelerometer taps, compass, and Health sleep state, exchange AppMessages. C covers every platform from Aplite through Gabbro; Alloy covers Emery and Gabbro only, which conflicts with Goal 4. Alloy remains the right choice if a Time 2-only prototype is wanted first to validate the feel in a weekend.

Constraints to design within: 24 KiB app RAM on Aplite, 64 KiB elsewhere; watchfaces have no button access; one watchface plus one watchapp sharing a persistent-storage schema.

### 10.2 Phone side: PebbleKit JS

Runs inside the Pebble mobile app while the face is displayed — which, for a watchface, is most of the day. Responsibilities: identity, network, coarse location quantisation, sleep-state relay, delivering bundles to the watch, storing preferences in `localStorage`.

Transport: WebSocket to the pond server if available in the Core app's JS runtime; otherwise long-poll every 60 s. Because the bundle cadence is already one per minute, polling is a complete fallback, not a degraded mode. **Open item:** verify WebSocket lifetime when the Pebble app is backgrounded on iOS.

### 10.3 Avatars: Pebble Draw Commands

A pebble is a triangle list — ≤ 24 triangles, points in a 48 × 48 box, one palette index each — rendered with the PDC API and animated by transforming points (tumble = rotate + squash). Storage: ~170 bytes per pebble. On the wire, pebbles travel as 4-byte seeds plus the three user choices; watch and server run the same deterministic generator, so a featured pebble costs ~7 bytes to send. On 1-bit displays the three colours map to three dither densities from the base PDC palette handling.

### 10.4 Server: Elixir / Phoenix

Phoenix Channels for the wrist connections, Presence for "who is awake in which shallow," one aggregator process per shallow emitting bundles on a 60 s tick, PubSub for fan-out. Postgres for the little that persists (§9). Expected scale is thousands of wrists and tens of messages per second at peak rain — one small VPS or a Fly machine. Estimated backend footprint: a few hundred lines.

### 10.5 Protocol sketch

```jsonc
// wrist → server
{ "t": "drop",  "seed": 1938211, "shallow": "eu-lyon", "echo_of": null }
{ "t": "echo",  "seed": 1938211, "shallow": "eu-lyon", "echo_of": "b_8812" }
{ "t": "state", "awake": true, "loc": true }

// server → wrist  (≤ 1 per minute)
{ "t": "bundle", "id": "b_8813", "n": 37, "near": 5, "far": 32,
  "bearing": 74, "dist": "far", "rain": false,
  "featured": [[551201, 3, 0, 7], [77102, 9, 2, 1], [9001, 1, 5, 4]] }

// server → wrist
{ "t": "reach",  "drop": "d_1", "wrists": 41, "echoes": 9 }
{ "t": "resonance", "with": 551201, "state": "new" }
```

### 10.6 Budgets

- **Watch battery**: one AppMessage and one short animation per minute at most; target < 3 % additional daily drain on Time 2, < 5 % on Aplite.
- **Bluetooth**: bundles under 64 bytes; no images, no geometry on the wire.
- **Phone**: measure background JS cost in the Pebble app on both platforms before beta.

## 11. Device support

| Watch | Display | Face + app | Ripples | Direction | Sound / Glow / Heartbeat |
|---|---|---|---|---|---|
| Pebble Time 2 (Emery) | colour, touch | ✓ | ✓ | ✓ | ✓ |
| Pebble Round 2 (Gabbro) | colour, touch | ✓ | ✓ | ✓ | glow only |
| Pebble 2 Duo (Flint) | 1-bit | ✓ (dithered pebbles) | ✓ | ✓ | — |
| Pebble Time / Steel / Round | colour | ✓ | ✓ | ✓ | — |
| Pebble 2 / Classic / Steel (Aplite, Diorite) | 1-bit | ✓ (reduced animation) | ✓ | ✓ | — |

Everything in the core loop works on every row. Extras are additive, never required.

## 12. Success criteria

Pond is working if, thirty days after public launch:

- ≥ 1,000 pebbles exist, and ≥ 40 % of wrists that installed still have Pond as their face (a watchface competes with every other face in the store; retention *as the face* is the honest metric).
- The median active wrist drops ≥ 2 times a day without being prompted.
- ≥ 15 % of ripples felt produce an echo.
- ≥ 100 resonance pairs have formed.
- **Health signals**: fewer than 10 % of wrists mute within a day of a ripple; fewer than 0.5 % of featured pebbles are reported.
- Qualitatively: people describe *feeling* a ripple to others. That sentence appearing unprompted on the community Discord or subreddit is the real launch signal.

## 13. Risks and open questions

| Risk | Mitigation |
|---|---|
| Accelerometer false positives (bumps, cycling) create phantom drops | Double-tap gate + magnitude threshold + 2 s cancel; daily hard limit caps damage |
| Empty pond at launch — nobody to feel | Launch *during* a rain with a coordinated community push; honest zero counter; make a single ripple feel excellent (§4.6). Never fake ripples. |
| pkjs socket does not survive the Pebble app being backgrounded | Polling fallback is a full substitute at the 60 s cadence; measure on iOS and Android early |
| Aplite RAM (24 KiB) vs PDC + animation | Reduced animation tier; pre-flattened pebbles; measured in beta |
| A rain causes a thundering herd | Rain cadence set server-side; bundles remain aggregated; Phoenix handles the fan-out |
| "Everyone" turns out to mean "spam" despite aggregation | The 1 / min ceiling is architectural, not a setting; large numbers are only celebrated during rain |
| Resonance becomes a dating vector | Direct ripples only, still no text, 1 / hour, either side dissolves silently |
| Does echo-within-10 s give enough of a loop? | Playtest with 30 people before building resonance; the answer decides §8.4's priority |

## 14. Roadmap

**v0 — private beta (target 6 weeks).** C watchface + app on Emery, Basalt, Diorite; drop, ripple, echo; on-watch pebble maker; quiet hours; Elixir backend; ~50 testers from the community Discord. Goal: confirm the feel and the false-positive rate.

**v1 — public.** All platforms including Aplite and Flint; direction and distance; reach view; reports and blocking; first rain. Store listing as a watchface, with the app bundled.

**v1.x.** Resonance; Time 2 sound, glow, and heartbeat; regional shallows tuning; a shareable pebble card (PNG rendered phone-side) for social posts.

**Not planned.** Text, photos, phone app, public API, monetisation.

## 15. Open decisions for the owner

1. C for reach (proposed) or Alloy for speed on Time 2 first?
2. Is heartbeat mode in v1.x or does it wait until resonance has proven itself?
3. Global pond only at launch, or shallows from day one? (Shallows make "near" ripples possible but thin out an already small pond.)