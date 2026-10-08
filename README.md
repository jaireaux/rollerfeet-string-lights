# Rollerfeet string lights

A Halloween light display running on a Seeed XIAO ESP32S3 with FastLED, a companion web app, and password-protected wireless firmware updates. Current installed firmware: **3.0.0-web.4**. The current code runs without Blynk.

## What it does now

- Drives **600 individually addressable WS2811 RGB pixels** using orange, purple, and green Halloween colors.
- Runs five animations: **Throb, Orange / Purple, Meteor Rain, Haunted Tide, and Witchfire Sparkles**.
- Provides a phone-friendly companion app at **[rollerfeet.com/lights](https://rollerfeet.com/lights/)**. The public page opens without a password to the animation picker, a countdown, and Next.
- Starts the selected animation immediately, resets its countdown, and continues cycling through all five. The current display uses 30 seconds per animation.
- Unlocks power, brightness, automatic cycling, playlist, and duration controls through **Admin Access Only**, using the web password.
- Supports **password-protected ArduinoOTA updates over Wi-Fi**, so firmware can be installed without reconnecting USB. It retries two configured Wi-Fi networks and keeps the lights running when the network is unavailable.
- Uses LED #1 for network and OTA status, returning it to the animation afterward; no-network status remains red.

Remote control goes through the IONOS website and an outbound relay on LNM to the controller. No home inbound port or public DDNS address is required. Runtime settings reset after controller restart. Browser firmware uploads are not implemented; OTA uses the Arduino-compatible upload workflow.

## Where to find the current code

**Use [`develop/3.0`](https://github.com/jaireaux/rollerfeet-string-lights/tree/develop/3.0) for the current firmware and companion app.** This default `main` branch preserves the original v2.0 source; its historical code still includes Blynk and does not provide the features described above.

- [Current README and Arduino workflow](https://github.com/jaireaux/rollerfeet-string-lights/blob/develop/3.0/README.md)
- [Web app operation and deployment](https://github.com/jaireaux/rollerfeet-string-lights/blob/develop/3.0/docs/web-control.md)
- [OTA setup and upload instructions](https://github.com/jaireaux/rollerfeet-string-lights/blob/develop/3.0/docs/ota.md)

## History: original v2.0 baseline

Imported existing source, with Wi-Fi name/password, Blynk template ID and auth token replaced by placeholders before the first commit. Original private files remain with the owner. Generated firmware/build output is excluded because it can contain embedded credentials. The import manifest records SHA-256 comparisons; the other 12 source files are byte-identical.

Saved build evidence points to Seeed XIAO ESP32S3 (likely Sense hardware). This is not yet a live chip identification. Source configuration: WS2811, RGB, GPIO 4, 300 pixels (6 × 50). Brightness varies by animation; the startup value is not a global cap.

Open `XIAO_ESP32_holiday_lights/XIAO_ESP32_holiday_lights.ino` directly from this checkout in Arduino IDE. The folder and main sketch names must match. All sibling .ino tabs belong to the sketch.

Dependencies observed at import: Espressif esp32 core 3.3.10, FastLED 3.10.5, Blynk 1.3.5. Compilation and hardware behavior have not yet been verified.

This import preserves legacy animation behavior and comments; it does not assert third-party authorship or introduce a blanket license.
