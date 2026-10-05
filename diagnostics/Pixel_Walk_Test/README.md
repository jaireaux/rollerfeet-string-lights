# Pixel Walk Test

Separate Arduino sketch; the main Halloween application is unchanged.
Open `Pixel_Walk_Test.ino`, select XIAO_ESP32S3 and its USB port, then upload.
Use Serial Monitor at 115200 baud. Uploading this test replaces the currently
running firmware; upload the main project afterward to restore the animations.

One dim white pixel moves forward every 250 ms, starting at pixel 1 (index 0).
Every update clears all other addressed pixels. The normal blacked-out connector
section is included. Serial output reports the commanded physical number and
zero-based index. There is no automatic LED-count detection or response channel.

`scanPixelCount` defaults to 1100, the total purchased inventory, so this test
can discover lights beyond the main program's current 300-pixel setting. A full
pass takes about 4 minutes 35 seconds and repeats. Once the scan goes beyond
the last actual pixel the string remains dark until the next pass. Reset the
board to restart at pixel 1. After measuring the run, set `scanPixelCount` to
its actual count for quicker repeated tests. If the chain exceeds this limit,
increase it before testing further.

Record the last visible pixel number and any dark or abnormal positions. An
unlit downstream section alone cannot distinguish the end of the string from
a broken data connection, failed pixel, or power problem.

Hardware: 12V WS2811 RGB pixels, XIAO D3/GPIO4 data, existing shared ground.
The XIAO still receives approximately 5V through its existing converter.
No Wi-Fi, OTA, Blynk, or wiring changes are part of this test.

Validation: XIAO ESP32S3 compile passed (413107 program bytes, 30248 global RAM bytes). A host simulation of the actual sketch passed all 1100 one-pixel steps, the 250 ms boundary, wraparound, inclusion of connector pixels, and timestamp rollover. No upload or physical test performed.
