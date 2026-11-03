# Controller platform proposal — 2026-11-02

Status: proposed, not assembled or electrically verified. Firmware remains 3.5.1; no code change tonight. Follow Notion DSHS for documentation and handoff.

## Observed problem and decision

Reloading the current 3.5.1 image and verifying restart did not restore the display. Johnny reports animation only on the first 8–9 pixels and incorrect colors. A data-path fault is suspected, not established by measurement. Rebuild/fortify the controller platform before string troubleshooting. Do not treat the apparent boundary as proof of a failed pixel.

## Layout

[Component-side placement diagram](../design/perfboard-controller-proposal.svg). Concept only; not a solder-side routing or hole-by-hole drawing. Actual B1 board dimensions, pad connectivity, enclosure clearance and XIAO header orientation must be checked before soldering. Keep antenna away from metal; USB accessible. Mount buck separately on the same insulated base if necessary. Pixel power current bypasses perfboard; connect grounds at an appropriate distribution point.

## Inventory evidence

Read Electronics Inventory Labeling chat (6ae81dfb-0ef8-83ea-8c57-28c740bfd848) and its retained 79-entry inventory at LNM /home/jaireaux/projects/Electronics/Spare-Electronics-Inventory/Inventory/Spare-electronics-inventory.txt. Identifications are visual, untested; repeated photos are not deduplicated stock quantities.

Confirmed categories: B1 double-sided prototype PCB assortment; B7 hookup wire; F6 26AWG silicone wire for short signal/low-current internal connections; B2/B3 splices; B5/E15 connector kits; B6/F8/F9 lever connectors; E6–E8 glands; E13 fasteners; A7 ties. Existing project photographs show three XIAO ESP32-S3 boards (two header-equipped, one directly wired), buck, power supplies and enclosure. Specific connector ratings/fit remain unverified.

## Parts not positively identified

- Two 1×7 female socket headers, 2.54mm pitch, accepting the XIAO's square male pins. Place rows 15.24mm apart center-to-center; dry-fit before soldering. Ordinary narrow DIP-14 socket is not for XIAO.
- SN74AHCT125N DIP-14 buffer; C1 contains unidentified DIP ICs, inspect before buying.
- DIP-14 socket, 7.62mm row spacing, for buffer.
- 100nF ceramic decoupling capacitor; C9 electrolytics do not confirm this part.
- 330Ω 1/4W series resistor, starting recommendation near first pixel DIN for short lead; revisit termination for actual cable length.
- 10kΩ buffer-input pulldown resistor.
- Suitable nylon spacers, PCB terminal blocks, controller-branch fuse/holder and external gel splice boxes; exact sizes/ratings pending wiring and enclosure.

## Buffer net list — component-side DIP numbering

14=regulated5V; 7=GND; 1=GND (enable channel1); 2=XIAO D3/GPIO4; 3=buffered data output. 100nF across14/7 with short leads. 10kΩ from2 toGND. Unused enables4/10/13 to5V; inputs5/9/12 toGND; outputs6/8/11 unconnected. Never expose XIAO or buffer to12V. Use common signal ground, cable strain relief and insulated supports; no mains wiring on perfboard.

References: https://www.ti.com/lit/ds/symlink/sn74ahct125.pdf ; https://www.seeedstudio.com/blog/2022/08/19/how-to-use-seeed-studio-xiao-in-your-pcb-design/ ; https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices

## Next session

Measure B1 board, identify C1 IC markings, confirm controller-to-first-pixel cable length, then produce exact assembly routing. Validate controller using a known-good short string before tracing installed data downstream. No hardware purchases or physical modifications performed.

Wrap-up recorded 2026-11-02 20:17 EST. Prompt receipt timing was not captured; no AI-wait duration invented. This is a documentation-only commit, not a firmware release.
