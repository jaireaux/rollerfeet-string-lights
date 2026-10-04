// One three-second cycle: orange rises, purple falls. Return the brightness
// for the controller to apply; no clock reads, delays, or LED output here.
uint8_t renderThrob(uint32_t elapsedMs) {
  static_assert(throbPeriodMs > 0 && throbPeriodMs % 2 == 0,
                "Throb period must contain two equal, nonzero halves");
  static_assert(throbMinBrightness <= outputBrightness,
                "Minimum brightness must not exceed the output limit");
  const uint32_t halfPeriodMs = throbPeriodMs / 2;
  const uint32_t phaseMs = elapsedMs % throbPeriodMs;
  const bool rising = phaseMs < halfPeriodMs;
  const uint32_t rampMs = rising ? phaseMs : throbPeriodMs - phaseMs;
  const uint8_t brightness = throbMinBrightness +
      (uint32_t(outputBrightness - throbMinBrightness) * rampMs) / halfPeriodMs;
  const CRGB color = rising ? CRGB::DarkOrange : CRGB::Purple;
  for (uint16_t i = 0; i < pixelCount; ++i) {
    pixels[i] = color;
  }
  return brightness;
}
