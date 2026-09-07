# Pond watchface

After joining through the Pebble phone app settings, try two short, quick wrist twists (a quick back-and-forth motion) to prepare a drop. Repeat the gesture within two seconds to cancel. In this build, the gesture feels more like a wrist twist than a screen tap: Pond uses accelerometer motion, not touchscreen input. There is no need to hit the glass. Recognition is experimental and may vary between wearers and watch models.

Run `pebble build` here to compile all seven targets. Install the bundle with `pebble install --emulator aplite` or a phone developer connection.

See the [root setup guide](../README.md), [protocol](../docs/PROTOCOL.md), and [implementation status](../docs/IMPLEMENTATION.md) for configuration, tests and physical-device gates. Set the service origin in `src/pkjs/env.js` before a real-phone build.

`src/c/` contains the native face and pure C comfort/gesture modules. `src/pkjs/` is plain ES5 executed by the Pebble phone app. Keep this source compatible with the SDK's older parser, including no trailing commas in function calls. No JavaScript dependencies, transpiler, or custom SDK build wrapper are required.

The UUID in `package.json` identifies Pond's installation and persistent storage. Keep it stable across builds. Normal SDK compilation generates the AppMessage key mappings from that manifest.

The little river stone comes from the approved [Figma mark](https://www.figma.com/design/84cY01bc17U2g9tjW3gz72?node-id=45-9). `resources/pebble.svg` is its vector source; `src/c/art-data.h` contains the checked-in integer paths. After changing the SVG, run `python3 resources/convert-art.py` from this directory. This optional converter uses Python's standard library; normal SDK builds need no extra step or dependency.

`src/c/art.c` draws those paths directly, with a fixed stack buffer and no per-frame allocation. Colour watches use a deep teal background, cream clock, and Warm gold or River teal stone; monochrome watches use only black and white. Rounded, Flat and Shard silhouettes retain the same quiet smile. Short live ripples settle into a static marker, reduced motion stays static, and offline water breaks into two banks. The character has no idle animation.

See the [native screenshot sheet](../design/store/screenshots-overview.png) for all seven targets. The clock stays in the original pixel glyphs, with room for AM/PM and the status label on both round and rectangular layouts.
