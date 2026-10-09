# Release notes

## 3.3.6 — 2026-10-09

- Offset quadrant pulses by quarter-cycle beats: red bottom-left, purple top-left, green top-right, orange bottom-right.
- Each full three-second Throb envelope starts 750 ms after the previous quadrant; per-quadrant brightness preserves full rise/fall and shared output controls.

## 3.3.5 — 2026-10-09

- Add bottom-right orange, synchronized with the other three quadrants on the original three-second Throb envelope.
- Keep shadow disabled, unmapped pixels dark and live cycling off.

## 3.3.4 — 2026-10-09

- Add green to the top-right quadrant, synchronized with red bottom-left and purple top-left on the same three-second Throb envelope.
- Keep bottom-right dark, shadow disabled and live cycling off.

## 3.3.3 — 2026-10-09

- Add purple to the mapped top-left quadrant, synchronized with the red bottom-left on the same three-second Throb envelope.
- Keep the right half dark, shadow disabled and live cycling off.


## 3.3.2 — 2026-10-09

- Use the original Throb three-second brightness ramp for the bottom-left red quadrant, via controller brightness (10–200, or 5–100% of effect peak).
- Extract a shared Throb envelope so both effects use the same algorithm; original Throb colors/timing/output are unchanged.
- Keep the quadrant red, outside pixels dark, shadow disabled and live cycling off.

## 3.3.1 — 2026-10-09

- Change Red Slosh phase-one diagnostic to pulse only the bottom-left quadrant of the mapped display.
- All selected pixels share a six-second 75–100% red pulse; other animation-buffer pixels are dark. Shadow remains disabled, status overlay unchanged.
- Preserve the spatial wave helper and prior releases for returning to the full-scene test.

## 3.3.0 — 2026-10-09

- Add a six-effect show playlist: original five animations plus Witch’s Brew, excluding Red Slosh. Expose it in admin controls and relay validation; use it at boot.
- Preserve the red-only troubleshooting renderer/checkpoint unchanged. Select Red Slosh and disable Auto cycle to resume.
- Picking an individual animation still resets its clock and enables the full seven-effect playlist.

## 3.2.2 — 2026-10-09

- Isolate Red Slosh’s red background for phase-one troubleshooting; comment out the shadow call and label background/shadow sections clearly.
- Preserve the shadow function for phase two. Tests verify active output never includes a shadow and test the preserved mask separately.
- Hold the live controller on Red Slosh only; automatic cycling must be restored explicitly after troubleshooting.

## 3.2.1 — 2026-10-09

- Slow Red Slosh to 24/37-second broad spatial waves so neighboring pixels vary smoothly.
- Render the full moving red background first, then apply the shadow in a separate pass. Preserve uncovered pixels exactly and continue waves during shadow pauses.
- Keep 75–100% red range, 20%-width shadow choreography and Witch’s Brew unchanged. User-reported blinking/freeze is not yet explained; visual retest required.

## 3.2.0 — 2026-10-09

- Add spatial Witch’s Brew: green liquid, expanding purple bubbles, short orange pops and soft-white steam in the upper third. Steam cores breathe 55–65% of effect peak, with soft spatial fades.
- Preview Red Slosh and Witch’s Brew; add seventh effect to picker and relay validation. Shared duration and private map provisioning remain unchanged.
- Appearance awaits dusk review.

## 3.1.0 — 2026-10-09

- Add Red Slosh: a draft 300-pixel spatial map, deep-red 75–100% waves, and a repeating 22-second vertical-shadow sequence. Unmapped addresses remain dark only in this effect.
- Add the sixth effect to the public/admin picker and relay validation; preview Red Slosh with Witchfire Sparkles.
- Preserve previous effects, shared duration, Wi-Fi/OTA and first-pixel status. Perceived appearance awaits dusk review.

## 3.0.2 — 2026-10-08

- Raise Haunted Tide’s maximum wave brightness from 75 to 101 (34.7%); keep the minimum, colors, fireflies and timing unchanged.

## 3.0.1 — 2026-10-08

- Move Haunted Tide to the second slot in the full playlist and website picker: Throb → Haunted Tide → Orange / Purple → Meteor Rain → Witchfire Sparkles.
- Keep the two-effect development preview unchanged.

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
