# Pond watchface

Run `pebble build` here to compile all seven targets. Install the bundle with `pebble install --emulator aplite` or a phone developer connection.

See the [root setup guide](../README.md), [protocol](../docs/PROTOCOL.md), and [implementation status](../docs/IMPLEMENTATION.md) for configuration, tests and physical-device gates. Set the service origin in `src/pkjs/env.js` before a real-phone build.

`src/c/` contains the native face and pure C comfort/gesture modules. `src/pkjs/` is plain ES5 executed by the Pebble phone app. Keep this source compatible with the SDK's older parser, including no trailing commas in function calls. No JavaScript dependencies, transpiler, or custom SDK build wrapper are required.

The UUID in `package.json` identifies Pond's installation and persistent storage. Keep it stable across builds. Normal SDK compilation generates the AppMessage key mappings from that manifest.
