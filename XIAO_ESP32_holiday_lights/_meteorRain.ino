// Rebuild one frame from elapsed time: no retained trail state or blocking sweep.
// Pixels keep their physical indices, including the dark connecting section.
uint8_t renderMeteorRain(uint32_t elapsedMs) {
  static_assert(meteorHeadPixels > 0 && meteorSpeedPixelsPerSecond > 0 &&
                meteorTrailFadeMs > 0 && meteorsPerCycle > 0, "Meteor size, speed and fade must be positive");
  static const CRGB colors[] = {
    CRGB(128, 70, 0), CRGB(64, 0, 64), CRGB(0, 50, 0)
  };
  // Keep the original travel/fade/pause cycle as the density reference.
  constexpr uint32_t headExitMs =
      ((uint32_t(pixelCount - 1) + meteorHeadPixels) * 1000 +
       meteorSpeedPixelsPerSecond - 1) / meteorSpeedPixelsPerSecond;
  constexpr uint32_t cycleMs = headExitMs + meteorTrailFadeMs + meteorLaunchPauseMs;
  constexpr uint32_t originalIntervalMs = (cycleMs + meteorsPerCycle - 1) / meteorsPerCycle;
  static_assert(originalIntervalMs > meteorLaunchAdvanceMs, "Launch interval must stay positive");
  constexpr uint32_t launchIntervalMs = originalIntervalMs - meteorLaunchAdvanceMs;
  constexpr uint32_t activeLaunches = (headExitMs + meteorTrailFadeMs + launchIntervalMs - 1)
      / launchIntervalMs;
  const uint32_t latestLaunch = elapsedMs / launchIntervalMs;
  const uint32_t latestAgeMs = elapsedMs % launchIntervalMs;
  constexpr uint64_t headLength = uint64_t(meteorHeadPixels) * 1000;
  constexpr uint64_t fadeDistance = uint64_t(meteorTrailFadeMs) * meteorSpeedPixelsPerSecond;

  for (uint16_t i = 0; i < pixelCount; ++i) {
    CRGB combined(0, 0, 0);
    const uint64_t pixelPosition = uint64_t(i) * 1000;
    // Reconstruct recent launches; never invent a meteor before animation start.
    for (uint32_t previous = 0; previous < activeLaunches; ++previous) {
      if (previous > latestLaunch) break;
      const uint32_t ageMs = latestAgeMs + uint32_t(previous) * launchIntervalMs;
      const uint64_t headPosition = uint64_t(ageMs) * meteorSpeedPixelsPerSecond;
      if (headPosition < pixelPosition) continue;
      const uint64_t distance = headPosition - pixelPosition;
      uint32_t intensity = 0;
      if (distance <= headLength) {
        intensity = 255;
      } else if (distance - headLength < fadeDistance) {
        intensity = 255 * (fadeDistance - (distance - headLength)) / fadeDistance;
      }
      const CRGB color = colors[(latestLaunch - previous) % 3];
      const CRGB contribution(uint16_t(color.r) * intensity / 255,
                              uint16_t(color.g) * intensity / 255,
                              uint16_t(color.b) * intensity / 255);
      // Keep the brighter contribution per channel, without additive brightening.
      if (contribution.r > combined.r) combined.r = contribution.r;
      if (contribution.g > combined.g) combined.g = contribution.g;
      if (contribution.b > combined.b) combined.b = contribution.b;
    }
    pixels[i] = combined;
  }
  return outputBrightness;
}
