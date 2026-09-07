# Pond — UI/UX design handoff

**Date:** 2026-09-07 · **Status:** first design iteration, ready for review and a physical watch prototype.

**Character revision, implemented 2026-09-07:** the approved smiling river stone, broken ripples, dark pond background and warm palette supersede the faceted-stone direction below. The native app retains the original pixel clock, state labels and comfort behaviour. Current implementation screenshots and banners are on Figma page **07 · Appstore assets** and in [the refreshed asset pack](store/README.md); earlier pages and exports remain the initial design study. See [watch artwork notes](../watch/README.md) for the vector source and conversion.

[Open the editable Figma file](https://www.figma.com/design/84cY01bc17U2g9tjW3gz72?node-id=16-1334)

The release contract remains [FIRST-RELEASE.md](../docs/FIRST-RELEASE.md). This design makes its watchface and phone settings concrete; it does not implement or deploy them.

## Design direction

Large, original pixel numerals; a small faceted stone; a still pond; one short state label. Watch screens use white and black with a restrained moss/teal palette on colour hardware. The warmer paper background and Manrope typography belong to phone settings and documentation, not to the watch palette.

The original bitmap glyph map is in [pixel-glyphs.json](pixel-glyphs.json). It is editable pixel artwork, not an approximation of a Pebble system font. At 144 pixels wide, the sample clock occupies 125 × 35 pixels. Status glyphs are 14 pixels high within an 18-pixel line; that small size must pass real glance-readability testing. All lettering and artwork use integer pixel steps. Phone copy uses Manrope; annotations use IBM Plex Mono.

Pebble's guidance recommends simple layouts, dominant information at a glance, and larger text. Custom fonts/resources are supported, but this artwork still needs conversion to bitmap resources or efficient integer drawing in C. [Design guidance](https://developer.repebble.com/guides/design-and-interaction/recommended/), [font API](https://developer.repebble.com/docs/c/Graphics/Fonts/).

## Display matrix

| Platform | Watches | Native display | Colour handling |
|---|---|---|---|
| Aplite | Classic / Steel | 144 × 168 rectangle | Black and white |
| Basalt | Time / Time Steel | 144 × 168 rectangle | 64 colours |
| Chalk | Time Round | 180 × 180 circle | 64 colours |
| Diorite | Pebble 2 | 144 × 168 rectangle | Black and white |
| Flint | Pebble 2 Duo | 144 × 168 rectangle | Black and white |
| Emery | Time 2 | 200 × 228 rectangle | 64 colours |
| Gabbro | Round 2 | 260 × 260 circle | 64 colours |

Verified against the [official hardware table](https://developer.repebble.com/guides/tools-and-resources/hardware-information/) on 2026-09-07. The seven platforms require five display layouts. Screens in the comparison are at native pixel dimensions; Figma zoom does not represent physical device size. Device listings are design targets, not claims of tested runtime compatibility.

Round compositions reposition the time, pebble and footer inside the circle; they do not crop a rectangular layout. All native colour channels are 00, 55, AA or FF. The exported monochrome screen contains only black and white. [Round design guidance](https://developer.repebble.com/guides/design-and-interaction/in-the-round/).

## Figma navigation

| Page | Contents |
|---|---|
| 00 · Pond | Cover and visual direction |
| 01 · Start here | Five native display layouts and platform constraints |
| 02 · Foundations | 39 scoped variables across three collections; Colour/Mono watch modes; six text styles; palette, spacing and original glyph specimens |
| 03 · Watchface | Three pebble silhouettes; 12 core watch states; nine adapted watch variants |
| 04 · States & gestures | Annotated state storyboard and linked gesture simulation |
| 05 · Phone settings | 14 settings screens; reusable action/setting components; join, comfort and leave flows |
| 06 · Handoff | Behaviour, implementation boundaries and physical verification gates |
| 07 · Appstore assets | Dashboard-sized icons, seven banners, and 14 actual emulator screenshots; see [the upload pack](store/README.md) |

The editable component families are Pond / Pebble, Pond / Watchface 144, Pond / Watchface adapted, Pond / Action and Pond / Setting. Avatar instances remain linked to their master components; colour semantics alias palette primitives. Fixed watch artwork geometry is intentionally positioned on a pixel canvas. Phone cards, rows, buttons and documentation use auto-layout.

## Watch experience

- **Still:** resting face, no ongoing animation or reminder.
- **Ripple:** a brief ring sequence, then a static recent marker; no countdown.
- **Echo:** the shared ripple received an answer; no claim of personal selection.
- **Drop:** lifted stone during the local two-second cancellation window.
- **Sending:** static pending cue; no success tick.
- **Dropped:** service acceptance, not proof another wearer felt it.
- **Unknown:** uncertain outcome; no failure buzz or automatic fresh request ID.
- **Offline:** disconnected/stale state has a broken pond line, distinct from an empty pond.
- **Setup:** local face works; joining happens through phone settings.
- **Paused / Quiet:** suppress haptics according to pause or quiet-hour policy.
- **Reduced motion:** static recent-ripple marker carries the same information.

The native watch screens have no touch or custom button targets. Current usage guidance is two short, quick wrist twists (a back-and-forth motion), based on Time 2 feedback. Repeat within two seconds to cancel. This is accelerometer motion, not a screen tap; gesture recognition is experimental and cannot be validated by Figma. Tap events alone do not expose magnitude; real samples and motor-settling handling require physical testing. [Accelerometer documentation](https://developer.repebble.com/guides/events-and-services/accelerometer/).

## Phone experience

The 390 × 844 frames depict a settings page inside the existing Pebble mobile app. They are not a separate native phone application or a claim to reproduce its exact platform chrome. Settings content can scroll; the action area stays visible. Primary actions are 52 pixels high, setting rows 80 pixels high, and Back targets 90 × 44 pixels. Keyboard focus is designed explicitly.

The key distinction is **saved on phone → waiting for watch → applied on watch**. Only an acknowledgement permits the applied state. Leaving shows watch participation and server deletion separately, including offline/pending outcomes. Back navigation in shared subpages returns to the prior settings state rather than claiming a different haptic mode.

The gentle-mode copy states: at most four incoming presentations per rolling 24 hours, at least 90 minutes apart, outside quiet hours. Pausing never generates a backlog. Silent remains the initial choice.

## Prototype scope

Use the Figma flow starts **Join silently & choose comfort**, **Leave & verify deletion**, and **Drop a pebble — simulated gesture**.

Core transitions are linked. External simulation controls for watch/server acknowledgements sit outside the product screens, so a timeout never masquerades as a successful save or deletion. The watch simulation similarly places its click controls outside the watch display. The two-second preparing transition illustrates the cancellation window only.

This is a review prototype, not a complete executable application. Detailed time pickers, palette/shape editing, endpoint entry, every combination of preference values, and native webview close actions are design layouts rather than fully emulated widgets. In particular, layout-only save actions must not be mistaken for tested persistence. No haptic feel, accelerometer reliability, network delivery, or battery claim follows from the prototype.

## Local artifacts

- [Watch family preview](previews/watch-family.png)
- [State storyboard](previews/storyboard.png)
- [Phone welcome](previews/welcome.png), [comfort](previews/comfort.png), [pebble customisation](previews/custom.png)
- Native-size editable SVG exports: [monochrome 144](mono-144.svg), [colour 144](colour-144.svg), [round 180](round-180.svg), [Time 2](time2-200.svg), [round 260](round-260.svg)
- [Original glyph map](pixel-glyphs.json)
- [Figma state ledger](figma/state.json) and [inspection results](figma/qa.json)

Files under `figma/` are authoring helpers and exact ID ledgers for this design session. They are reference snippets for the Figma tool, not a standalone application or a complete replay script. Final exports and the live Figma components are the visual source of truth. There is no Code Connect mapping because no app implementation exists yet.

## Verification and remaining work

Inspected rendered screenshots of all display layouts, the 12-state storyboard, foundations, phone screens and the watch simulation. Fixed multiline text sizing, round-screen overlap, clipped ripple rings, quiet-hour wording, keyboard focus and Back hit areas. Local SVG checks verify native dimensions and hardware-valid palettes, including pure black/white for monochrome.

Figma inspection found 39 scoped variables, six text styles, no unexpected phone font families, no missing destinations among the configured prototype links, and no undersized action/setting instances. This checks design structure, not a completed user journey on the device.

Before implementation is considered ready: test glyph legibility at real size, 12/24-hour display and AM/PM placement, localised strings, system overlays, larger phone text, assistive-technology semantics, round clipping, battery impact, motion false positives and background phone execution. The prototype's two-minute recent marker must become a static marker after its short animation in code; it must not animate continuously.

The next useful experiment is a C watchface with the resting state, a local simulated ripple and the gesture recogniser on physical monochrome and newer hardware.
