#include "throb_envelope.h"
// Each color gets a full three-second rise and fall. Return the brightness
// for the controller to apply; no clock reads, delays, or LED output here.
uint8_t renderThrob(uint32_t elapsedMs) {
  static_assert(throbPeriodMs > 0 && throbPeriodMs % 2 == 0,
                "Throb period must contain two equal, nonzero halves");
  static_assert(throbMinBrightness <= outputBrightness,
                "Minimum brightness must not exceed the output limit");
  const uint8_t brightness=throbBrightness(elapsedMs,throbPeriodMs,throbMinBrightness,outputBrightness);
  static const CRGB colors[] = {
    CRGB(128, 70, 0), // Darker orange: half the previous DarkOrange intensity.
    CRGB(64, 0, 64),  // Darker purple: half the previous Purple intensity.
    CRGB(0, 50, 0)    // Dark green.
  };
  const uint8_t colorIndex = (elapsedMs / throbPeriodMs) % 3;
  const CRGB color = colors[colorIndex];
  for (uint16_t i = 0; i < pixelCount; ++i) {
    pixels[i] = color;
  }
  return brightness;
}
