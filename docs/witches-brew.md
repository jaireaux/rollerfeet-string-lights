# Witch’s Brew — simulator adaptation (3.4.1)

Reference: [operator-submitted source](simulator-sources/witches-brew-3.4.1.js).
This is the supplied edited source, not the simulator’s original preset. The
preview’s 25×12 serpentine map is replaced with the installation’s private
`spatial_map.h` (x rightward, y upward). Existing unmapped addresses remain black.

`brewSurfaceAt(x, simmer)` sets the shared 0.80 ± 0.04 surface. The pool and each
bubble use that function. Each bubble’s upper edge approaches the local surface
as its cycle reaches 1, then resets. Orange pops retain the submitted local
surface + 0.08 offset. Steam remains in the upper third, with its two widths
increased from 0.20/0.13 to 0.30/0.195. Liquid and steam therefore overlap between
roughly two thirds and four fifths of scene height, as in the submitted source.

Frame helpers compute background first and additive steam second. Fractional
RGB is retained between layers; channels are rounded/clamped once at output,
matching the browser rather than the previous renderer’s truncating casts.
Bubble positions and trigonometric frame phases are calculated once per frame.
Each period uses elapsed milliseconds modulo its own period; widened arithmetic
preserves bubble offsets near the uint32_t boundary. No persistent effect state
or new scheduler/reset hook is needed. Existing 30 ms cadence, playlist IDs,
600 addresses, brightness cap, Wi-Fi/OTA and first-pixel status remain unchanged.

Validation: `CXX=c++ node tests/witches_brew_parity.mjs` compares a C++ numerical
probe against the submitted JavaScript at 4,641 point/time combinations, including
late elapsed time. Allow one RGB level for float versus JavaScript double arithmetic.
The dedicated host test checks surface/endpoints, steam placement, unmapped
pixels, repeatability and absence of FastLED output calls. The physical hardware
still adds its configured brightness limit and optical behavior; numerical parity
is not a visual observation.


## 3.4.2 — Shared surface at 85%

The liquid boundary is 0.85 + 0.04 × sin(2πx + simmer), measured upward from the bottom of the physical map. Bubble tops target this local boundary, and orange pop flashes are centered on it. The earlier +0.08 pop offset is removed. Steam still occupies its existing upper-third region and overlaps the liquid; timing, brightness and playlists are unchanged. The revised numerical reference is simulator-sources/witches-brew-3.4.2.js; the operator’s original 3.4.1 submission is preserved.
