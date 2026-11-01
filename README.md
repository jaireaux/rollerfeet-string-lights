# Rollerfeet String Lights

A fall light display on a Seeed XIAO ESP32-S3, with a companion website and password-protected OTA updates. **Installed release: 3.5.0.**

## What it does now

- Rotates **Harvest Glow → Autumn Tide → Orchard Bands → Falling Embers → Cranberry Drift → Harvest Cider**, three minutes each.
- Uses the six approved simulator fall recipes: muted orange, olive, gold, cranberry, amber and cream. Witchfire Sparkles is not in this rotation; Halloween code remains in Git/source for reference.
- Cranberry Drift has a **20-second color loop**, 4-second rotation, 24-second horizontal travel, 8-second vertical travel and 6-second throb. Its shadow is disabled; the original helper remains available.
- Harvest Cider uses a shared 85% liquid/bubble surface, olive bubbles, golden pops and warm cream steam.
- Addresses **600 WS2811 RGB outputs**. Spatial effects use the existing private 300-pixel map and leave unmapped outputs black. Global output limit remains 200/255, with the website brightness setting at 100%; the blackened connecting section remains disabled.
- [rollerfeet.com/lights](https://rollerfeet.com/lights/) opens without a password to the six-animation picker, countdown and Next. Picking an effect resets its countdown and resumes automatic cycling. Admin controls retain power, brightness, playlist and duration.
- Password-protected ArduinoOTA, two-network Wi-Fi retries and LED #1 network/OTA status remain in place. Remote web commands use the existing outbound LNM/IONOS relay.

## 3.5.0 verification and rollback

OTA upload and restart verified: version 3.5.0, uptime 1,812 ms, OTA ready. All six selections accepted after installation; final state power on, brightness 100%, automatic full playlist, duration 180 seconds, animation 0. Physical outdoor appearance remains for the operator to verify.

Build: ESP32 core 3.3.12 / FastLED 3.10.5; 1,117,071 program bytes and 58,332 global RAM bytes. Host C++ tests, six-effect controller integration and web-control tests pass. 66,402 simulator/C++ samples match within one RGB level except two exact quadrant-edge samples verified to match adjacent-coordinate rounding. Existing Brew parity still passes 4,641 samples. Exact approved simulator 0.1.25 recipes are retained in [docs/simulator-sources/fall-3.5.0](docs/simulator-sources/fall-3.5.0/).

Private LNM OTA image/helper: `/var/lib/sentinelx/uploads/holiday-lights-ota/firmware-3.5.0.bin` and `deploy-3.5.0.py`; active `deploy.py` pins 3.5.0. Prior 3.4.2 firmware and `deploy-3.4.2.py` remain for rollback. Firmware contains credentials and is excluded from Git and public/cloud source exports. Image SHA-256: `e5a50e6135faec41496e8258badc50fd0cb3d212382d0497c185c6a7b593f5bc`.

Source: [GitHub](https://github.com/jaireaux/rollerfeet-string-lights), tag v3.5.0. LNM retained Git: `/media/jaireaux/backup/projects/holiday-lights/`. Drive: `gdrive-jaireaux:LNM/Projects/holiday-lights/`. Main human-facing project record: [Notion](https://app.notion.com/p/3ef76785d91981a2a848da74e9b3f626).

## Release versions

Firmware and companion website use the same release number. Patch releases (3.0.1) cover fixes and small adjustments; minor releases (3.1.0) add compatible features or animations; major releases (4.0.0) introduce breaking changes. Each release receives a matching Git tag and GitHub release. See [release notes](CHANGELOG.md).

For every firmware or website rollout, update this README to reflect current behavior and limitations, update the firmware and website version together, and record changes in CHANGELOG.md before publishing. Verify the installed version after deployment and retain the release source.

## Historical 3.4.2 verification

The following is the previous release record, retained for reference. Current installed firmware is 3.5.0.

Previous firmware: **3.4.2**. Password-protected ArduinoOTA and two-network Wi-Fi retry support are implemented; the lights continue while Wi-Fi is unavailable. A read-only `/status` endpoint reports the running version. The root page provides password-protected web controls; see [web control setup](docs/web-control.md). Version ota.1 was installed over USB, then ota.2 was successfully uploaded over Wi-Fi and read back after reboot. 3.4.2 build: esp32 core 3.3.12, FastLED 3.10.5, 1,113,019 program bytes and 58,332 global RAM bytes. Nine host C++ tests plus production playlist, submitted-JavaScript numerical parity, and web-control tests pass. OTA read-back verifies 3.4.2; the restored all-seven show starts on Witch’s Brew with power on, brightness 100% and 180-second duration. Outdoor visual validation remains for the operator. Latest quadrant/controller host tests and web-control tests passed; prior full checks cover retry timing/rollover, animation output pausing and the production playlist. LED #1 shows Wi-Fi/OTA status, with OTA priority and brief success confirmations. Offline red remains until the next search or connection. Browser firmware upload and BLE controls are future work. See [OTA setup and upload instructions](docs/ota.md).

## Web controls

Web.1 was prepared on October 6 while the controller was unreachable. Web.2 was installed wirelessly on October 7 and its restarted status was verified. It shortens Meteor Rain launches to 1.25 seconds. The IONOS page and LNM relay now reach the controller.

The phone-friendly page offers all seven Halloween animations, on/off, brightness, next animation, automatic cycling, preview/full playlist, and a shared 10–600 second animation duration. Boot defaults to all seven effects at 180 seconds each; installed runtime uses the same settings. User changes still reset to these defaults on controller restart. Off and brightness zero leave the first pixel available for network/OTA status.

The public IONOS page opens with animation selection, Next, and a countdown. “Admin Access Only” prompts for the existing web password before showing full controls. Remote access uses an HTTPS relay on IONOS and a persistent outbound-only Python service on LNM. It requires no home inbound port or published DDNS. The page waits for controller acknowledgement, refuses commands while offline, and expires pending commands. [Operation and deployment](docs/web-control.md). Browser firmware upload, Tuya switch control, and BLE remain future work.

## Firmware profiles and animation implementation

Provision the private `spatial_map.h` from your retained layout before building; the generic `spatial_map.example.h` is only a starting point. Open `XIAO_ESP32_holiday_lights/XIAO_ESP32_holiday_lights.ino` in Arduino IDE; select `esp32:esp32:XIAO_ESP32S3`. Boot now starts all seven effects; the Red Slosh / Witch’s Brew preview remains selectable. Selecting any animation enables all seven. Preview mode remains available for trying two effects while adding animations. Production retains Throb, Haunted Tide, Orange / Purple, Meteor Rain, Witchfire Sparkles, Red Slosh, Witch’s Brew. All effects use one `animationDurationMs`. `HOLIDAY_LIGHTS_PRODUCTION` defaults to 0; both profiles now use 180 seconds per effect. This replaces the earlier per-effect 9/16-second durations. Production runs three minutes per effect; preview remains selectable, and the shared duration can be shortened through the web controls.

The animation table contains names, frame intervals, and render functions. `AnimationClock` checks elapsed time and frame deadlines. `updateAnimation(now)` advances the playlist, starts the next effect immediately, applies its brightness and calls `FastLED.show()` once. Effects never block, call delay, or send their own frames. Late frames sample current elapsed time without replaying missed frames.

| Effect | Behavior | Frame interval |
|---|---|---:|
| Throb | Darker orange, purple, green; full 3-second rise/fall for each, brightness 10–200 | 50 ms |
| Orange / Purple | Whole visible string switches between darker orange and purple every second, brightness 200 | 1000 ms |
| Meteor Rain | Two staggered meteors with smooth fading trails; successive launches use darker orange, purple, green | 20 ms |
| Haunted Tide | Slow Halloween-colored waves with scattered fireflies that gently brighten and fade | 40 ms |
| Witch’s Brew | Green simmer, purple bubbles and orange pops, with 55–65% white steam in the upper third | 30 ms |
| Red Slosh | Spatial red pulses at 30–100%, combined drift/sine/rotation, with a 20%-wide shadow and midpoint pauses | 30 ms |
| Witchfire Sparkles | Whole string gently throbs and shifts orange/purple/green, with 1–3 warm-white glints per 25 pixels | 30 ms |

Witchfire Sparkles is a new renderer with a three-second whole-string throb and smooth six-second transitions between each Halloween color. Each region flashes 1–3 distinct sparkle positions for one nominal 30 ms displayed frame, then returns to the background on the next frame. Regions fire once every 240 ms with staggered timing; there is no fade. Glints use 75% of their original brightness (`sparkleBrightnessPercent=75`), without dimming the background. Actual flash duration depends on frame delivery. Positions/counts are deterministic pseudorandom choices from elapsed time; no frame history or global random state. Tune `sparkle_settings.h`. The saved connector mask is currently disabled.

Haunted Tide combines four Pacifica-inspired wave layers with one independent firefly per 10-pixel region. Fireflies glow for 1.8–3.2 seconds, pause for 0.7–2 seconds, then choose a new position and Halloween color within their region. Wave colors are dimmer than the fireflies, with soft colored highlights instead of white flashes. Tune `haunted_tide_settings.h`. The original Pacifica source remains preserved in `legacy/`; this new renderer uses elapsed time and the shared controller.

Meteor settings live in `meteor_settings.h`: `meteorHeadPixels=10`, `meteorSpeedPixelsPerSecond=60`, `meteorTrailFadeMs=800`, `meteorLaunchIntervalMs=1250`. Each meteor starts 1.25 seconds after the previous launch, independent of string length. Rendering reconstructs every pixel from elapsed time, without changing speed or trail length. Recent launches are rendered together, keeping the brighter contribution per color channel. New launches can start before earlier trails finish; there is no shared all-dark pause. The meteor traverses all physical indices; restoring the connector mask will hide it across that section. The controller can end this effect at the common deadline without waiting for a sweep.

RGB colors are orange (128,70,0), purple (64,0,64), and green (0,50,0). Hardware uses GPIO4/XIAO D3, WS2811/RGB, 600 pixels in both profiles, TypicalLEDStrip correction, and global brightness limit 200. Production selects the full playlist and three-minute animation duration. The saved black connecting section uses zero-based indices 210–251; its mask call is commented out pending outdoor installation.

Refactored Classic Christmas is preserved outside the build in `inactive-animations/`. Original source is preserved under `legacy/pre-classic-refactor/`. Dependencies: esp32 core 3.4.0, FastLED 3.10.5. Nonblocking Wi-Fi retries across two configured networks and password-protected ArduinoOTA are implemented. BLE and browser firmware upload remain future work. See [OTA instructions](docs/ota.md).

Run each `tests/*_test.cpp` with `c++ -std=c++11 -Wall -Wextra -pedantic -I tests/fakes`, then execute its output. Also run `playlist_test.cpp` with `-DHOLIDAY_LIGHTS_PRODUCTION=1`. Tests exercise the actual main controller/renderers using fake LED output, plus timing rollover and the preserved Classic Christmas effect. On-device appearance still requires upload and visual checking.

## History: v2.0 baseline

Imported existing source, with Wi-Fi name/password, Blynk template ID and auth token replaced by placeholders before the first commit. Original private files remain with the owner. Generated firmware/build output is excluded because it can contain embedded credentials. The import manifest records SHA-256 comparisons; the other 12 source files are byte-identical.

Saved build evidence points to Seeed XIAO ESP32S3 (likely Sense hardware). This is not yet a live chip identification. Source configuration: WS2811, RGB, GPIO 4, 300 pixels (6 × 50). Brightness varies by animation; the startup value is not a global cap.

Open `XIAO_ESP32_holiday_lights/XIAO_ESP32_holiday_lights.ino` directly from this checkout in Arduino IDE. The folder and main sketch names must match. All sibling .ino tabs belong to the sketch.

Dependencies observed at import: Espressif esp32 core 3.4.0, FastLED 3.10.5, Blynk 1.3.5. Compilation and hardware behavior have not yet been verified.

This import preserves legacy animation behavior and comments; it does not assert third-party authorship or introduce a blanket license.

## History: initial v3.0 setup (before Classic Christmas refactor)

Work on `develop/3.0`. The current sketch needs no credentials and does not connect to Wi-Fi or Blynk. The Wi-Fi-only `arduino_secrets.example.h` is reserved for future connectivity; private credentials remain in ignored `arduino_secrets.h`. Never commit credentials or compiled firmware containing them. Version: 3.0.0-dev. At that setup stage animation behavior had not yet been refactored.

## Arduino and Git workflow

Open the sketch inside this Git checkout and always save there. Arduino edits the files; Git versions those same files. Close or save Arduino tabs before switching branches and reopen afterward. Do not edit an older separate copy and expect Git to see it.

Before work: `git pull --ff-only`. After saving: `git diff`, `git add` the intended source files, `git commit -m "Describe the change"`, then `git push`. Configure board and port in Arduino separately; uploading firmware is distinct from committing source.

Use Git to share committed code with the LNM clone. Treat Google Drive as a one-way retained snapshot, not an independently edited working folder. No unattended synchronization is configured. Credentials must be provisioned separately on each machine.

[Timing records](docs/timing.md) describe measured project age and AI waiting.

## Historical build and refactoring notes

Build verification (2026-10-04): Arduino CLI compile for esp32:esp32:XIAO_ESP32S3 passed using the observed dependency versions: 1,274,219 bytes program storage and 50,992 bytes global RAM. No upload or live hardware validation performed.

Blynk removal: removed the Quickstart callbacks, uptime reporting, cloud connection and timer. Serial debugging and all animation code, pixel settings, brightness settings and animation intervals are retained. The lights no longer wait for a Blynk/Wi-Fi connection at startup. Wi-Fi controls and OTA are not implemented yet. Animation rendering cadence may change without network servicing; hardware validation is still required.

Blynk-free build verification (2026-10-04): PASS for `esp32:esp32:XIAO_ESP32S3`, esp32 core 3.4.0 / FastLED 3.10.5. Program storage: 676,743 bytes; global RAM: 27,892 bytes. All 12 animation/helper files and animation dispatch match the previous commit. No upload or live test performed.

Classic Christmas refactor validation (2026-10-04): both host tests passed; XIAO ESP32S3 compile passed (666,787 bytes program, 27,700 bytes global RAM). All 13 preserved legacy files match their previous committed bytes. No firmware upload or hardware test performed.

Throb validation (2026-10-04): all three host tests passed; XIAO ESP32S3 compile passed (412,743 bytes program, 27,856 bytes global RAM). Shared scheduler unchanged and refactored Classic Christmas preserved byte-for-byte. No firmware upload or hardware test performed.

Three-color Throb validation (2026-10-04): all three host tests passed, including constant color through both halves of each pulse and the nine-second restart. XIAO compile passed (412,759 bytes program, 27,856 bytes global RAM). No upload performed.

Two-animation validation: all four host tests passed; XIAO compile passed (412,935 bytes program, 27,864 bytes global RAM). Tests cover intervals/transitions and rollover, Throb, alternating color boundaries, and preserved Classic Christmas. No upload or visual hardware test performed.

Meteor Rain validation: all five host test programs passed, including controller/rendering integration in development and production modes. XIAO ESP32S3 compile passed (413,267 program bytes, 27,864 global RAM bytes). No upload or physical visual check performed.

Blackout mask temporarily disabled: all 600 pixels participate in animations. The saved indices 210–251 and mask function remain for restoring the connecting section after installation.

## Temporary pixel mapping test

The 3.0.2-pixel-walk.2 diagnostic uses 400 pixels, one full RGB-white bulb at brightness 255 with no color correction for 250 ms, repeating in approximately 100 seconds. Keep the camera fixed and capture pixel 1 and one entire pass so bulb numbers can be matched to positions. Wi-Fi and OTA remain available; the status overlay is suppressed. Normal website animation controls do not select effects during this temporary test. Restore release 3.0.2 over OTA afterward. The reusable source change is retained in diagnostics/Pixel_Walk_Test/mapping-overlay.patch; apply it to a private copy of the 3.0.2 sketch, which uses the existing private Wi-Fi/OTA configuration. The standalone original test remains unchanged.
