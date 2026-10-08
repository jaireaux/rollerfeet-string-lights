# Release notes

## 3.0.0 — 2026-10-08

First supported 3.0 milestone, consolidating the development versions through 3.0.0-web.4.

- Five Halloween animations on a 600-pixel WS2811 display; shared timing and nonblocking rendering.
- Public companion website with animation picker, actual countdown, and Next. Selection resets the timer and continues the full playlist.
- Password-protected admin power, brightness, cycling, playlist and duration controls.
- Remote HTTPS control through IONOS and an outbound LNM relay, with controller acknowledgements and offline protection.
- Password-protected ArduinoOTA updates, two-network Wi-Fi retry, and first-pixel network/update status.
- Blynk removed from current code; original source and historical development evidence preserved.

Runtime settings reset after controller restart. The development boot profile starts the two-animation preview at 30 seconds; production profile starts all five at 180 seconds. The current installed display is set to all five at 30 seconds. Selecting an animation enables the full playlist.

Build and host tests verify scheduling, commands, renderers and network status. Browser visual QA remains unavailable; phone appearance is reviewed by the operator.
