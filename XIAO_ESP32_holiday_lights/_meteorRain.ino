// Rebuild one frame from elapsed time: no retained trail state or blocking sweep.
// Pixels keep their physical indices, including the dark connecting section.
uint8_t renderMeteorRain(uint32_t elapsedMs) {
  static_assert(meteorHeadPixels > 0 && meteorSpeedPixelsPerSecond > 0 &&
                meteorTrailFadeMs > 0, "Meteor size, speed and fade must be positive");
  static const CRGB colors[] = {
    CRGB(128, 70, 0), CRGB(64, 0, 64), CRGB(0, 50, 0)
  };
  // Wait until the head passes the last pixel, then its trail fades, then pause.
  constexpr uint32_t headExitMs =
      ((uint32_t(pixelCount - 1) + meteorHeadPixels) * 1000 +
       meteorSpeedPixelsPerSecond - 1) / meteorSpeedPixelsPerSecond;
  constexpr uint32_t launchPeriodMs = headExitMs + meteorTrailFadeMs + meteorLaunchPauseMs;
  const uint32_t phaseMs = elapsedMs % launchPeriodMs;
  const CRGB color = colors[(elapsedMs / launchPeriodMs) % 3];
  // Position in thousandths of a pixel, avoiding frame-rate-dependent movement.
  const uint64_t headPosition = uint64_t(phaseMs) * meteorSpeedPixelsPerSecond;
  constexpr uint64_t headLength = uint64_t(meteorHeadPixels) * 1000;
  constexpr uint64_t fadeDistance = uint64_t(meteorTrailFadeMs) * meteorSpeedPixelsPerSecond;

  for (uint16_t i = 0; i < pixelCount; ++i) {
    uint32_t intensity = 0;
    const uint64_t pixelPosition = uint64_t(i) * 1000;
    if (headPosition >= pixelPosition) {
      const uint64_t distance = headPosition - pixelPosition;
      if (distance <= headLength) {
        intensity = 255;
      } else if (distance - headLength < fadeDistance) {
        intensity = 255 * (fadeDistance - (distance - headLength)) / fadeDistance;
      }
    }
    pixels[i] = CRGB(uint16_t(color.r) * intensity / 255,
                     uint16_t(color.g) * intensity / 255,
                     uint16_t(color.b) * intensity / 255);
  }
  return outputBrightness;
}
