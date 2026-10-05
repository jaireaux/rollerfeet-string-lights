# Wireless firmware uploads

The lights use the ESP32 core's password-protected ArduinoOTA service. Blynk is not required. The current animation pair remains Meteor Rain / Witchfire Sparkles. The original animation renderers do not perform network I/O.

## Private setup

The first network uses `SECRET_WIFI_SSID` / `SECRET_WIFI_PASSWORD` in ignored `XIAO_ESP32_holiday_lights/arduino_secrets.h`. Copy the example header if setting up another checkout. Optional network two and the OTA password belong in ignored `ota_secrets.h`; use `ota_secrets.example.h` as the template. Use an OTA password of at least 12 characters. Without a configured first network and OTA password, the lights run with networking disabled. Empty alternate credentials disable network two. Never commit these files or compiled firmware containing credentials.

The service tries each configured network for 10 seconds, then waits 20 seconds before another round. Animations continue during connection attempts and while disconnected. After disconnection it first retries the last successful network. Both networks must provide a route between the uploading computer and the lights; guest isolation can prevent uploads even when Wi-Fi is connected.

## First installation and verification

1. Compile and upload the OTA-capable sketch once over USB using board `esp32:esp32:XIAO_ESP32S3` and an OTA-capable partition scheme. The selected default build contains two application slots of 0x330000 bytes each.
2. The Serial Monitor (115200) reports firmware version, device IP, and OTA readiness. Secrets are not logged.
3. Open `http://holiday-lights.local/status` or use the reported device IP. This read-only JSON endpoint reports firmware version, uptime, and OTA readiness; browser upload is a later step.
4. Compile a distinguishable new version retaining OTA support. In Arduino IDE choose the `holiday-lights` network port and upload, supplying the private OTA password when asked. The Mac and device must be reachable on the same LAN.
5. Verify the new version on `/status` after reboot. The USB data cable can then be removed while the controller remains powered by the existing external supply.

Keep the installed ESP32 core's uploader paired with its ArduinoOTA implementation. This project was built against core 3.3.12; its uploader supports the current authentication protocol.

## Command-line upload

From the repository root:

```sh
python3 tools/ota_upload.py --ip DEVICE_IP --file PATH_TO_APPLICATION.bin
```

The helper imports the installed ESP32 uploader and reads the OTA password locally without placing it on the command line. On macOS it locates installed cores under the standard Arduino data directory; elsewhere supply `--espota PATH_TO_ESP32_TOOLS/espota.py`. Upload the application `.bin`, not the merged flash image. The upload uses device UDP port 3232 and a callback TCP listener on computer port 3240; allow the uploader through the computer firewall if prompted. Compiling firmware still happens on a computer; iPhone browser upload will be added separately.

## Update behavior and recovery

During an actual update, animation rendering is suspended; pixels #2 onward hold their last frame while pixel #1 indicates OTA status. The ESP32 shows green for two seconds after successful flashing, then restarts. A handled update error releases the pause and restarts the current animation's clock. Wi-Fi loss restarts connection attempts. Firmware that cannot boot still requires USB recovery; automatic rollback is not enabled by this change.

The local status endpoint is read-only and does not grant update access. ArduinoOTA requires the private password. These are LAN services; do not expose them through router port forwarding. Future work: authenticated browser updater and optional password-protected fallback access point. Witch's Cauldron remains the animation backlog item after OTA.

Official references: [ArduinoOTA example](https://github.com/espressif/arduino-esp32/blob/3.3.12/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino), [browser OTA workflow](https://docs.espressif.com/projects/arduino-esp32/en/latest/ota_web_update.html).

## First-pixel status indicator

Physical LED #1 (code index 0) temporarily overlays the animation. OTA indications take priority over network indications. The animation buffer stays intact, and status brightness is independent of the animation brightness.

| State | Indication | Duration |
|---|---|---|
| Searching for a configured network | Blinking blue | Connection attempts |
| Network connected | Solid blue | 2 seconds |
| No configured network connected after a round of attempts | Solid red | 4 seconds, then animation during retry cooldown |
| Receiving an OTA update | Blinking green | While transfer makes progress |
| OTA image successfully written | Solid green | 2 seconds, then reboot |
| OTA error | Blinking red | 4 seconds |
| No current indication | Animation | Until the next event |

Blink half-period is 250 ms. During a stalled OTA transfer, callbacks stop, so blinking can pause until transfer processing resumes; it is not an independent hardware heartbeat. Changing one WS2811 pixel still sends a frame to the entire string, adding some transfer overhead. A dead controller, power loss, or broken data path cannot be reported by this indicator. Green confirms the image was written successfully; the status endpoint verifies that the new firmware actually boots.
