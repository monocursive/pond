# Pond — Appstore asset pack

[Editable Figma page](https://www.figma.com/design/84cY01bc17U2g9tjW3gz72?node-id=34-2) · [Screenshot overview](screenshots-overview.png)

Designed on 2026-09-07 against the live [Pebble Developer Dashboard](https://developer.repebble.com/dashboard), using the published benzaiflow listing to inspect each platform's upload requirements. Revised the same day with a more playful identity. That listing was not changed. No Pond listing or release has been created or published.

## Upload map

| Dashboard field | File | Dimensions |
|---|---|---|
| Small Icon | [pond-80.png](icons/pond-80.png) | 80 × 80, recommended by dashboard |
| Large Icon | [pond-144.png](icons/pond-144.png) | 144 × 144, recommended by dashboard |
| Upload New Banner, per platform | `banners/pond-{platform}-720x320.png` | Exactly 720 × 320 |
| Add New Screenshots, per platform | `screenshots/{platform}-still.png`, then `{platform}-ripple.png` | Exactly the native sizes below |

Every upload file is PNG. The [512-pixel master](icons/pond-512.png), [editable vector mark](source/pond-mark.svg), and screenshot overview are supporting assets, not screenshot-slot uploads. No separate companion-app icon is needed for Pond's current PebbleKit JS architecture: settings run through the existing Pebble phone app.

| Dashboard tab | Platform | Screenshot size | Banner |
|---|---|---|---|
| Pebble Classic | aplite | 144 × 168 | [PNG](banners/pond-aplite-720x320.png) |
| Pebble Time | basalt | 144 × 168 | [PNG](banners/pond-basalt-720x320.png) |
| Pebble Time Round | chalk | 180 × 180 | [PNG](banners/pond-chalk-720x320.png) |
| Pebble 2 | diorite | 144 × 168 | [PNG](banners/pond-diorite-720x320.png) |
| Pebble Time 2 | emery | 200 × 228 | [PNG](banners/pond-emery-720x320.png) |
| Pebble 2 Duo | flint | 144 × 168 | [PNG](banners/pond-flint-720x320.png) |
| Pebble Round 2 | gabbro | 260 × 260 | [PNG](banners/pond-gabbro-720x320.png) |

Choose the matching tab before uploading its banner and screenshots. Upload the icons once in App Icons. Keep screenshot uploads unframed; the banners include an illustrated watch surround. Round captures retain the emulator's transparent exterior pixels within their square image bounds.

## Design

A slightly lopsided river stone has a small smile and floats over loose, broken ripples. Its sloping highlight and off-centre features give it a recognisable expression at small sizes. The character is content to be there: no pleading, anxious or demanding expressions. The exported square icons have opaque backgrounds and generous internal margins; any store cropping can happen outside the mark. The transparent SVG mark is intended for a dark ink background.

The promotional palette is deep pond ink (`#163F39`), butter (`#FFF1C7`), ochre (`#E4B96F`), stone shadow (`#BD864B`) and soft water (`#70B9A6`). A bold lowercase Fraunces Black wordmark pairs with Manrope body copy and IBM Plex Mono footnotes. All lettering remains editable in Figma. The voice is “A little plop. A little company.”, followed by “Nothing needs answering.”

The native watch now draws the character from this vector artwork, adapted to integer paths and Pebble's hardware palette. It keeps the original pixel clock and state labels. Colour watches have deep teal backgrounds and cream lettering; monochrome watches have white artwork on black. The watch surrounds are illustrations, not product photographs. Each banner includes the corresponding platform's native screenshot without scaling it. The first geometric icon and clean banner are retained to the right of the current work on the Figma page, labelled Archive / v1.

## Screenshot provenance

All 14 screenshot files were refreshed from the local Pond 0.1.0 character implementation on 2026-09-07, using Pebble Tool 5.0.40 / SDK 4.33.1. The implementation is not yet committed; `manifest.json` records the captured bundle and watch-source hashes. These are actual emulator pixels, with no painted-over labels, reconstructed UI, resizing or colour correction. The Figma proof sheet and seven banner images were refreshed from these same captures.

The capture session supplied local AppMessage fixtures for joined/silent settings and the Still and Ripple states. Gentle vibration and confirmation ticks were disabled, reduced motion was enabled, and the ripple was supplied as a silent snapshot. The phone bridge was not registered with a public pond. These images demonstrate the watch renderer; they do not prove live network delivery, haptic comfort, battery life or physical-device readiness. Each emulator was returned to a locally unjoined state after capture.

To refresh after rebuilding the watch, run `python3 design/store/source/capture-screenshots.py` from the repository. This uses only the standard SDK and Python library, reads the generated message-key map, installs on all seven emulators, and replaces the 14 PNGs. Updating these files does not automatically update the images already placed in Figma or the banner exports.

`figma-nodes.json` records the editable source nodes. `manifest.json` records expected dimensions and SHA-256 checksums of the deliverable PNGs and SVG. The archive contains this pack; upload its individual images to the dashboard.

Public release still depends on the [implementation gates](../../docs/IMPLEMENTATION.md). Asset preparation is not publication.
