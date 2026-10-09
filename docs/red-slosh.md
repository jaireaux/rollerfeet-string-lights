# Red Slosh

Version 3.1.0 uses a provisional 300-pixel map derived from the operator’s pixel-walk video. The private coordinate lookup lives in ignored `spatial_map.h` (provision it from the private retained copy before building; `spatial_map.example.h` is a generic three-point illustration): index 0 corresponds to physical pixel #1; x is left-to-right and y bottom-to-top over the mapped display bounds. #26/#27 are proportional interpolations between #20/#30 following the operator’s correction. Foliage-obscured points remain provisional. Photo/video evidence is retained privately on LNM and Google Drive, not in this repository.

Two broad sine waves move in different directions across x/y. Deep red RGB (255,0,4) fluctuates within 75–100% of its peak before shadowing. Existing global output limit 200, strip color correction and user brightness remain in force; “100%” is relative to this effect’s configured peak. Addresses 301–600 are black in this mapped effect, without changing existing effects or the controller’s 600-address output.

The vertical shadow occupies 20% of mapped scene width with soft edges and a black center. Travel is one second from fully off-scene to fully off-scene; halfway travel takes half a second. Red motion continues beneath it.

| Time (seconds) | Action |
|---|---|
| 0–7 | Red waves only |
| 7–8 | Shadow left to right |
| 8–11 | Wait right |
| 11–12 | Shadow right to left |
| 12–15 | Wait left |
| 15–15.5 | Move left to midpoint |
| 15.5–16.5 | Pause midpoint |
| 16.5–17 | Return left |
| 17–20 | Wait left |
| 20–20.5 | Move left to midpoint |
| 20.5–21.5 | Pause midpoint |
| 21.5–22 | Continue right |

The shadow choreography repeats every 22 seconds; wave phases continue independently. The shared animation duration still controls playlist switching. A 120-second duration permits five full sequences and a partial sixth. Development preview is Red Slosh then Witchfire Sparkles; full playlist appends Red Slosh after the original five. Choosing it in the picker restarts the effect and continues automatic cycling. Visual validation at dusk remains necessary; corrections to the coordinate lookup do not require redesigning the renderer.

## Witch’s Brew — 3.2.0

Uses the same private map. A green pool occupies the lower roughly 40% with a rolling surface; three staggered purple bubbles expand and rise, with short orange bursts near the surface. Soft-white wisps drift only in the upper third (normalized y > 2/3). Wisp cores breathe from 55% to 65% around a 60% midpoint, relative to the existing output cap and user brightness; soft edges are dimmer. Broad wisps accommodate the sparse pixel layout. A 15-second simmer phase combines with independently staggered 6.2/7.3/8.4-second bubble periods and 9-second steam motion, avoiding one obvious synchronized loop. Addresses beyond the map remain dark. Preview now pairs Red Slosh with Witch’s Brew at the shared duration. Dusk appearance review remains pending.

## Red Slosh smoothing — 3.2.1

Following operator reports of wild blinking outside the shadow and apparent freezing during shadow sweeps, waves now use broad 24/37-second periods and lower spatial frequencies. `renderRedSloshBackground()` sets every mapped pixel from the continuous x/y field; `applyRedSloshShadow()` then blacks the band center and softens its edges, preserving uncovered pixels exactly. Background wave phases never depend on shadow state. Tests verify bounded temporal/spatial change, unchanged uncovered colors and continued motion during a one-second midpoint pause. The previous formula also advanced continuously, so the observed hardware behavior has no confirmed root cause yet. Preserve 75–100% red range, output cap and existing 22-second shadow timing; user visual retest remains necessary.

## Phase-one isolation — 3.2.2

User supplied IMG_6524.MOV and requested two-phase troubleshooting. The red and shadow sections are labeled separately; `applyRedSloshShadow(elapsedMs)` is commented out in the active renderer. Red waves remain 24/37 seconds at 75–100% of their peak. The preserved shadow function is tested separately but does not run. Live controller selects Red Slosh and disables automatic cycling; runtime settings reset after reboot, so reapply the manual hold after an upload or restart. Other animations remain available, but are not in the active rotation. First observe the red-only display, then resolve any remaining red flicker before restoring the shadow. The 22-second supplied daylight clip is evidence, not proof of a particular root cause; camera motion/exposure limit precise comparison.

## Saved checkpoint and six-effect show — 3.3.0

The operator paused troubleshooting for an errand. Preserve Red Slosh’s separate background/shadow sections and commented-out shadow call. Prior live checkpoint: 3.2.2, physical addresses 600 (mapped 300), Red Slosh ID 5, auto off, power on, brightness 100%, duration 120 seconds. Resume by selecting ID 5, then disabling Auto cycle. Pending question: does red-only output still flicker? Then troubleshoot the red field before restoring the shadow.

The active show instead cycles IDs 0,1,2,3,4,6: Throb, Haunted Tide, Orange / Purple, Meteor Rain, Witchfire Sparkles, Witch’s Brew. New playlist value 2 reports `show`; value 0 remains the two-effect preview, value 1 all seven. The show skips Red Slosh on automatic transitions and Next, while direct picker selection deliberately enables all seven. Boot defaults to show; runtime duration remains configurable and resets after restart.

## Bottom-left quadrant test — 3.3.1

Phase-one output now selects x < 0.5 and y < 0.5 over mapped display bounds (y=0 is bottom), then gives every selected pixel the same six-second cosine throb at 75–100% of the configured red peak. All other animation-buffer positions are dark, including unmapped addresses; the network/OTA first-pixel overlay retains priority. Shadow call remains commented out. The old spatial wave helper remains available but is not used by this diagnostic background. Live controller held on Red Slosh with automatic cycling disabled. Test goal: observe whether a uniform small-area pulse behaves correctly before returning to spatial motion and then shadowing.

## Original Throb envelope — 3.3.2

Operator confirmed quadrant placement but reported no visible change. Replaced the small 75–100% six-second per-pixel modulation with the actual original Throb algorithm: three-second linear rise/fall, brightness 10→200→10 (5–100% of configured peak). Both Throb and Red Slosh now call `throbBrightness()` in throb_envelope.h. The red quadrant buffer stays RGB(255,0,4); the renderer returns envelope brightness for the shared controller/output path to apply. Original Throb still changes only its existing colors after complete cycles. Tests cover envelope endpoints and prove displayed red rises from 12 to 255 in the pre-global-output buffer. Shadow stays disabled; outside/unmapped pixels black. Live Red Slosh held alone; visual confirmation pending.
