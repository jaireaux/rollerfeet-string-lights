# Rollerfeet string lights

Arduino holiday lights using FastLED. The v2.0 baseline retains its historical Blynk integration; v3.0 runs locally without Blynk.

## Current development: Classic Christmas only

The active Arduino sketch now contains only Classic Christmas. Other animations and the previous main sketch are preserved byte-for-byte under `legacy/pre-classic-refactor/`, outside the sketch directory so Arduino does not compile them. They remain historical Blynk-free source, not standalone build targets.

Open `XIAO_ESP32_holiday_lights/XIAO_ESP32_holiday_lights.ino` in Arduino IDE. Select `esp32:esp32:XIAO_ESP32S3`. Current dependencies: esp32 core 3.3.10 and FastLED 3.10.5. No Blynk or network credentials required. Wi-Fi controls and OTA are not implemented yet.

The main sketch owns hardware settings and `updateAnimation(now)`. `animation_clock.h` decides when to render and restart. `_classicChristmas.ino` draws one frame from elapsed time; it never delays, reads the clock, or sends LED data. Output masking and `FastLED.show()` happen once in the controller.

| Setting | Value | Meaning |
|---|---:|---|
| `animationDurationMs` | 15000 | Restart the sole active animation after 15 seconds |
| `frameIntervalMs` | 500 | Minimum time between rendered frames |
| `classicChristmasStepMs` | 500 | Advance the palette offset every half second |
| `outputBrightness` | 200 | Preserve the old Classic Christmas brightness |
| `pixelCount` | 300 | Existing configured count, not purchased inventory |

GPIO4 (XIAO D3), WS2811, RGB order and TypicalLEDStrip correction are retained. Zero-based pixels 210 through 251 remain black. The seven colors retain their original order. Intentional timing changes: first frame is immediate; every restart begins with red at pixel zero; late calls render the current time-derived pattern instead of slowing the effect or replaying missed frames. No animation table or other effect migration is introduced yet.

Host checks:

```sh
c++ -std=c++11 -Wall -Wextra -pedantic tests/animation_clock_test.cpp -o /tmp/holiday-clock-test && /tmp/holiday-clock-test
c++ -std=c++11 -Wall -Wextra -pedantic tests/classic_christmas_test.cpp -o /tmp/holiday-classic-test && /tmp/holiday-classic-test
```

These check frame deadlines, lifetime restart, delayed calls, timestamp rollover, and all 300 palette positions. A successful board compile does not substitute for an on-device visual check.

## v2.0 baseline

Imported existing source, with Wi-Fi name/password, Blynk template ID and auth token replaced by placeholders before the first commit. Original private files remain with the owner. Generated firmware/build output is excluded because it can contain embedded credentials. The import manifest records SHA-256 comparisons; the other 12 source files are byte-identical.

Saved build evidence points to Seeed XIAO ESP32S3 (likely Sense hardware). This is not yet a live chip identification. Source configuration: WS2811, RGB, GPIO 4, 300 pixels (6 × 50). Brightness varies by animation; the startup value is not a global cap.

Open `XIAO_ESP32_holiday_lights/XIAO_ESP32_holiday_lights.ino` directly from this checkout in Arduino IDE. The folder and main sketch names must match. All sibling .ino tabs belong to the sketch.

Dependencies observed at import: Espressif esp32 core 3.3.10, FastLED 3.10.5, Blynk 1.3.5. Compilation and hardware behavior have not yet been verified.

This import preserves legacy animation behavior and comments; it does not assert third-party authorship or introduce a blanket license.

## Historical v3.0 setup (before Classic Christmas refactor)

Work on `develop/3.0`. The current sketch needs no credentials and does not connect to Wi-Fi or Blynk. The Wi-Fi-only `arduino_secrets.example.h` is reserved for future connectivity; private credentials remain in ignored `arduino_secrets.h`. Never commit credentials or compiled firmware containing them. Version: 3.0.0-dev. At that setup stage animation behavior had not yet been refactored.

## Arduino and Git workflow

Open the sketch inside this Git checkout and always save there. Arduino edits the files; Git versions those same files. Close or save Arduino tabs before switching branches and reopen afterward. Do not edit an older separate copy and expect Git to see it.

Before work: `git pull --ff-only`. After saving: `git diff`, `git add` the intended source files, `git commit -m "Describe the change"`, then `git push`. Configure board and port in Arduino separately; uploading firmware is distinct from committing source.

Use Git to share committed code with the LNM clone. Treat Google Drive as a one-way retained snapshot, not an independently edited working folder. No unattended synchronization is configured. Credentials must be provisioned separately on each machine.

[Timing records](docs/timing.md) describe measured project age and AI waiting.

Build verification (2026-10-04): Arduino CLI compile for esp32:esp32:XIAO_ESP32S3 passed using the observed dependency versions: 1,274,219 bytes program storage and 50,992 bytes global RAM. No upload or live hardware validation performed.

Blynk removal: removed the Quickstart callbacks, uptime reporting, cloud connection and timer. Serial debugging and all animation code, pixel settings, brightness settings and animation intervals are retained. The lights no longer wait for a Blynk/Wi-Fi connection at startup. Wi-Fi controls and OTA are not implemented yet. Animation rendering cadence may change without network servicing; hardware validation is still required.

Blynk-free build verification (2026-10-04): PASS for `esp32:esp32:XIAO_ESP32S3`, esp32 core 3.3.10 / FastLED 3.10.5. Program storage: 676,743 bytes; global RAM: 27,892 bytes. All 12 animation/helper files and animation dispatch match the previous commit. No upload or live test performed.

Classic Christmas refactor validation (2026-10-04): both host tests passed; XIAO ESP32S3 compile passed (666,787 bytes program, 27,700 bytes global RAM). All 13 preserved legacy files match their previous committed bytes. No firmware upload or hardware test performed.
