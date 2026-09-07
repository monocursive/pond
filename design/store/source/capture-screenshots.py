"""Capture actual Pond renders with local, silent AppMessage fixtures.

Run from any directory after `cd watch && pebble build`.
Requires the standard Pebble SDK; never connects to physical watches or publishes.
"""

import json
from pathlib import Path
import subprocess
import time

ROOT = Path(__file__).resolve().parents[3]
WATCH = ROOT / "watch"
OUTPUT = ROOT / "design/store/screenshots"
KEYS = json.loads((WATCH / "build/js/message_keys.json").read_text())
PLATFORMS = json.loads((WATCH / "package.json").read_text())["pebble"]["targetPlatforms"]
SETTINGS = dict(Type=1, Revision=100, Joined=1, Gentle=0, QuietStart=0,
                QuietEnd=0, AllDay=0, PauseUntil=0, Reduced=1, Tick=0,
                Seed=1, Shape=0, Palette=0)


def run(platform, command, *args):
    subprocess.run(["pebble", command, "--emulator", platform, *args],
                   cwd=WATCH, check=True, timeout=45)


def message(platform, values):
    run(platform, "send-app-message", "--uint",
        *[f"{KEYS[key]}={value}" for key, value in values.items()])


OUTPUT.mkdir(parents=True, exist_ok=True)
for platform in PLATFORMS:
    run(platform, "install")
    try:
        message(platform, SETTINGS)
        message(platform, dict(Type=2, Bundle=0, Expires=0, Other=0,
                               Snapshot=1, ServerTime=int(time.time())))
        run(platform, "screenshot", "--no-open", "--no-correction",
            str(OUTPUT / f"{platform}-still.png"))
        # Render the static recent-ripple state without sound or haptics.
        message(platform, dict(Type=2, Bundle=1, Expires=int(time.time()) + 120,
                               Other=1, Echo=0, Snapshot=1,
                               ServerTime=int(time.time())))
        run(platform, "screenshot", "--no-open", "--no-correction",
            str(OUTPUT / f"{platform}-ripple.png"))
    finally:
        # Leave the emulator locally unjoined after the capture session.
        message(platform, dict(SETTINGS, Revision=101, Joined=0))
    print(f"Captured {platform}", flush=True)
