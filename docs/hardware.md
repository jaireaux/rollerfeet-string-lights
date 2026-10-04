# Hardware identification and build

Saved build artifacts identify esp32:esp32:XIAO_ESP32S3. A supplied camera-board photograph is consistent with XIAO ESP32S3 Sense, but photographs and saved build settings do not prove the connected chip model. At initial intake no live chip identification had been performed. Subsequent user-supplied esptool v5.3.1 flash-id output on 2026-10-04 identifies ESP32-S3 (QFN56), revision v0.2, 8 MB embedded PSRAM, 8 MB quad flash at 3.3 V, 40 MHz crystal and USB-Serial/JTAG. This identifies the chip, not the installed firmware or exact carrier-board SKU. The temporary RAM stub and reset did not replace the installed application. No project firmware has been uploaded.

In Arduino IDE, inspect the board selector or Tools > Board and Tools > Port. Select XIAO_ESP32S3 for this board family after confirming the hardware. USB auto-detection may report only ESP32 Family Device or an unrelated board sharing a USB identifier.

With the board attached to the terminal host, close Serial Monitor and use esptool v5: `esptool --port PORT flash-id`. The connection output reports the chip model. This reads identification, not firmware writes, but normally resets the board and briefly interrupts the running program. In Termius SSH, the USB board must be attached to the SSH host. Merely opening Termius on a different device does not expose that other device's USB port.

Observed toolchain: esp32 core 3.3.10, FastLED 3.10.5, Blynk 1.3.5. FQBN: esp32:esp32:XIAO_ESP32S3. Do not treat default compile options as verified upload settings.

References:
- https://support.arduino.cc/hc/en-us/articles/4406856349970-Select-board-and-port-in-Arduino-IDE
- https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/
- https://docs.espressif.com/projects/esptool/en/latest/esp32s3/esptool/basic-commands.html
