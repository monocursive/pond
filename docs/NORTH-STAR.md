# Pond — North star

**Version:** 0.3, draft · **Date:** 2026-09-07 · **Owner:** Michaël / Monocursive

**Purpose:** Preserve the spirit of [the original v0.2 proposal](archive/PRODUCT-SPEC-v0.2.md), while making comfort, truthful feedback, and sustainable maintenance part of the design. [FIRST-RELEASE.md](FIRST-RELEASE.md) defines what we build first.

## 1. The feeling

“I am not the only one wearing this odd little watch right now.”

Pond is a Pebble watchface where each wearer has a small, faceted pebble. A wearer drops theirs into a shared pond. Other participating wearers may see the ripple or, when they have invited it, feel it on their wrist.

It is playful, gentle, and slightly strange: a small gift to the people keeping Pebble alive. A ripple never asks for a reply. An empty pond is honest. A quiet wearer belongs just as much as an active one.

**The complete experience can be one pebble, one ripple, and a smile.** Additional features must improve that experience without making silence feel like absence or failure.

## 2. Name and vocabulary

Pond stays. It is small enough for a single stone to matter, and completes the pebble metaphor. Naming availability should be checked again before publication; the historical naming research is not a current availability guarantee.

| Term | Meaning |
|---|---|
| Pebble | A generated visual avatar, recognisable through silhouette and facets as well as colour. It is not an authentication credential. |
| Drop | A deliberate contribution to the shared pond. |
| Ripple | A visual or haptic presentation of an aggregated bundle of recent contributions. A network update does not necessarily cause a vibration. |
| Echo | A deliberate reply to a shared ripple. It acknowledges that bundle, not a randomly assigned person. |
| Reach | Optional future information about confirmed watch deliveries, never a claim that someone felt, read, or enjoyed them. |
| Shallows | Optional future coarse geographic context within the same global pond. |
| Resonance | A possible future, mutually accepted connection between recognisable pebbles. |
| Rain | An optional, brief shared event that people explicitly join. |

## 3. Principles

1. **Comfort belongs to the wearer.** Silent participation is complete. Haptics are invited, bounded, and easy to stop.
2. **Attention is scarce.** Delivery cadence and vibration cadence are separate. Popularity never increases the interruption budget.
3. **Nothing is owed.** No streaks, response deadlines, read receipts, missed-event prompts, competitive scores, or obligation to participate.
4. **Small numbers matter.** One other person is enough. No fabricated activity, replacement bots, or simulated participants on the live pond.
5. **Feedback is truthful.** Accepted is not delivered; delivered is not felt; replying to a group is not choosing a person.
6. **The watch remains a watch.** Time is legible, the resting face is still, and all essential information works without animation, vibration, sound, or colour discrimination.
7. **Explain control plainly.** The social surface has no user-authored text. Settings, permissions, errors, deletion, and accessibility instructions may use clear text.
8. **Old watches belong.** The long-term goal is the whole Pebble family. Published support reflects actual verification and may arrive in stages.
9. **Collect for a stated purpose.** Each stored item has a retention period. Privacy promises describe the real system, including logs and backups.
10. **The community can keep it alive.** Source, release artifacts, deployment instructions, and a configurable server endpoint are part of the gift.

## 4. The everyday experience

### Arrival

Install the face and receive a generated pebble immediately. Customisation is optional. The first version starts silent and offers a brief explanation of joining, gesture controls, and the optional gentle haptic mode through the existing Pebble mobile app's settings.

An on-watch maker remains desirable: silhouettes, a few palette choices, small expressions, and a shuffle action. It must wait for a proven interaction and identity-sharing design. A watchface cannot simply acquire button access, and a separate watchapp does not automatically share its state.

### Resting face

Time dominates. Your pebble sits below it. A small indicator can show fresh activity or connectivity without becoming a counter to chase. Zero activity and unavailable information must remain visually distinct.

An arriving ripple may briefly expand around the pebble. Reduce-motion mode uses a static marker. A future featured visitor is supplementary; the experience must remain meaningful without recognising anyone or memorising symbols.

### A deliberate drop

A validated gesture gives immediate visual feedback. The network later confirms whether the drop was accepted. Cancellation should happen locally before transmission wherever possible. Accidental motion and the watch's own vibration must not create contributions.

Gesture recognition is a physical-device experiment, not a promise supplied by the tap API. If reliable recognition requires unacceptable battery use or excludes too many wrists, revise the interaction before widening release.

### A ripple and its echo

An incoming ripple says that other people contributed. A wearer can ignore it indefinitely. While a recent ripple remains available, a deliberate drop can reply to that bundle. There is no countdown and no bonus for answering quickly. Once the context expires, the gesture is simply a new drop.

An echo must never be attributed to a featured person unless the wearer actually selected that person. Feedback to contributors can say that their shared ripple received an answer. It cannot say that someone chose them.

## 5. Comfort contract

The first-release policy is deliberately conservative and defined precisely in [FIRST-RELEASE.md](FIRST-RELEASE.md). Its initial values are hypotheses for testing, not medically validated limits:

- Silent by default; gentle haptics are an explicit choice.
- At most four unsolicited haptic presentations in any rolling 24 hours, at least 90 minutes apart.
- Quiet hours default to 22:00–08:00 local and apply to every Pond haptic. System quiet modes are also respected wherever detectable; lack of reliable detection must be disclosed and addressed in release testing.
- No morning catch-up buzz, reconnection burst, escalating reminder, or compensating vibration after mute ends.
- Echo feedback shares the incoming budget. Direct connections and events cannot silently bypass the budget in a later version.
- User-triggered previews or accepted-drop confirmations are optional, short, and rate-limited separately. Their presence never overrides silent mode or quiet hours.
- No automatic sound or backlight activation in the core experience.
- Budget state survives restarts. Missing or untrustworthy time/state leads to silence rather than a reset allowance.

Haptic design uses distinct on/off timings supported by the portable Vibes API. Words such as “soft,” “swell,” and “patter” describe the desired impression, not guaranteed amplitude control. Evaluate each pattern on old ERM and newer LRA hardware before claiming support.

## 6. Possible later experiences

### Pebble maker and companion watchapp

Retain the faceted, low-poly character and a limited set of friendly choices. On monochrome watches, use deliberate patterning or high-contrast facets; automatic colour-to-dither conversion is not assumed.

A companion may eventually host the maker and optional history. It needs its own packaging, lifecycle, and explicit linking mechanism. The first release must not depend on installing it.

### Reach

If people miss knowing whether a ripple arrived, explore a private, coarse delivery summary. Count deduplicated watch acknowledgements only. Use plain terms such as “delivered to watches”; never “people felt you.” Do not add prompts to check it, comparative ranking, or goals.

### Shallows and direction

Explore only if geography adds warmth in playtests. Keep one global pond so geography does not isolate a small community. Location is separately opt-in, quantised on the phone, short-lived, and unnecessary for participation. Small cohorts must not expose a recognisable person's whereabouts.

Hardware without a compass receives a suitable alternative. Neither a compass nor optional location may become a core-loop requirement.

### Resonance

Repeated bundle echoes are not evidence that two individuals chose one another. A future connection needs a deliberate selection and mutual acceptance, without public rejection, urgency, or an “unlock” threshold.

Direct ripples are separately invited, share the wearer's overall comfort policy, and can be disabled or dissolved silently. There is no last-seen status or delivery pressure. A proposal must include abuse handling and demonstrate why the connection improves the toy before implementation.

### Rain

A shared celebration remains a strong possibility. Join explicitly for that particular event; participation is never automatic through installing Pond or having joined previously.

Explore a one-to-two-minute session, with visual richness and at most a brief opening and closing pattern if the wearer has opted into event haptics. Quiet hours and mute still win. No 15-second repeating haptic loop, autoplay, missed-event notification, or reward for attendance. Exact budgets require a separate reviewed event specification.

### Sound, glow, and heartbeat

Keep as uncommitted experiments behind proven hardware/API availability. Sound and glow each require separate permission. Heartbeat waits until mutually accepted resonance proves useful and the data lifecycle is specified. No global heart-rate collection is planned for the first release.

## 7. Architecture direction

**Watch:** C SDK with capability checks and explicit monochrome/round layouts. Build and measure against each platform's actual SDK limits; Aplite's 24 KiB code-plus-heap budget is an early constraint.

**Phone bridge:** PebbleKit JS inside the existing Pebble mobile app. It owns authenticated HTTPS, optional configuration, and message translation. Its lifecycle follows the watch application. Neither polling nor WebSockets guarantee execution when the runtime is suspended.

**Service:** Elixir / Phoenix, Ecto, and PostgreSQL. Begin with one global aggregator under OTP supervision and ordinary HTTP endpoints. Channels, Presence, regional processes, clustering, and PubSub are options when the product needs them, not prerequisites.

**Identity:** a random installation identity with a revocable server credential. Avatar seeds are separate. Watch tokens are app-specific and tied to hardware; they are not proof of one human, authentication secrets, or a cross-application linking scheme.

**Protocol:** explicit versioning, idempotent submissions, expiring bundles, and watch-owned haptic decisions. Compact AppMessage encoding is measured including dictionary overhead. Document the protocol so people can maintain or replace the service.

## 8. Privacy, safety, and continuity

No accounts, names, chat, uploads, advertising, commercial analytics, or public activity history. Fixed generated avatars reduce expressive abuse; they do not make misuse impossible. Authenticate actions and bound registration, requests, and contributions. Never describe installation rate limits as verified limits per human.

Location, health, personal connections, reports, and longer histories need separate retention decisions before introduction. Do not carry those tables or permissions into the first release pre-emptively.

Leaving the pond is an explicit, authenticated action in settings. Uninstalling is not a reliable server deletion signal. Explain what is removed immediately, what expires later, and what cannot be retracted from already delivered anonymous aggregates.

Community hosting includes practical backups, restoration, deletion handling, server replacement, and a graceful service shutdown. The watchface keeps time and its local pebble without a server, while clearly showing that live activity is unavailable.

## 9. What success means

Pond succeeds when people welcome it on their wrists, feel comfortable ignoring it, and find it easy to make quieter. A silent user is a successful user. A small, happy community is sufficient.

Learn through voluntary beta conversations and bounded technical measurements. Do not infer exact installed-face retention, satisfaction, or physical awareness from polling. Do not optimise for drops per day, echo conversion, connection counts, or low mute usage.

Evidence to seek: time remains easy to read; people understand control and delivery limits; normal motion does not cause phantom drops; quiet periods remain quiet; battery impact is acceptable; someone else can run the service; the maintainer still enjoys the project.

## 10. Decisions and promotion rule

Confirmed direction: **Elixir / Phoenix backend; C watchface; one global pond; optional haptics; no companion watchapp required for first release.**

Later features enter a release only with a demonstrated wearer benefit, a defined comfort budget, a truthful feedback model, a bounded data lifecycle, and an affordable maintenance cost. Keeping Pond small is an acceptable permanent outcome.

## Documentation references

Reviewed on 2026-09-07; implementation must still validate behaviour on current phones and watches.

- [PebbleKit JS lifecycle, storage isolation, and watch tokens](https://developer.repebble.com/guides/communication/using-pebblekit-js/)
- [App metadata and watchface classification](https://developer.repebble.com/guides/tools-and-resources/app-metadata/)
- [Persistent storage](https://developer.repebble.com/docs/c/Foundation/Storage/)
- [Accelerometer callbacks and vibration-contaminated samples](https://developer.repebble.com/guides/events-and-services/accelerometer/)
- [Portable Vibes API](https://developer.repebble.com/docs/c/User_Interface/Vibes/)
- [Hardware information](https://developer.repebble.com/guides/tools-and-resources/hardware-information/)
- [Platform-specific capabilities, including Diorite's lack of compass](https://developer.repebble.com/guides/app-resources/platform-specific/)
- [Clay configuration](https://developer.repebble.com/guides/user-interfaces/app-configuration/)
