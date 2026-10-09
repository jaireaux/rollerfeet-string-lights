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
