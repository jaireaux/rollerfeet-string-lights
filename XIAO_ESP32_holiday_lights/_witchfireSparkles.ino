#include <math.h>

uint32_t sparkleHash(uint32_t value) {
  value ^= value >> 16;
  value *= 0x7feb352dU;
  value ^= value >> 15;
  value *= 0x846ca68bU;
  return value ^ (value >> 16);
}

CRGB sparkleBackground(uint32_t elapsedMs) {
  static const CRGB colors[] = {
    CRGB(128, 70, 0), CRGB(64, 0, 64), CRGB(0, 50, 0)
  };
  const uint8_t index = (elapsedMs / sparkleColorShiftMs) % 3;
  float blend = float(elapsedMs % sparkleColorShiftMs) / sparkleColorShiftMs;
  blend = blend * blend * (3.0f - 2.0f * blend); // Ease through color transitions.
  const float pulse = 0.5f - 0.5f * cosf(6.28318530718f *
      float(elapsedMs % sparkleThrobMs) / sparkleThrobMs);
  const float level = (sparkleBackgroundMin +
      (sparkleBackgroundMax - sparkleBackgroundMin) * pulse) / 255.0f;
  const CRGB &a = colors[index];
  const CRGB &b = colors[(index + 1) % 3];
  return CRGB(uint8_t((a.r*(1-blend)+b.r*blend)*level),
              uint8_t((a.g*(1-blend)+b.g*blend)*level),
              uint8_t((a.b*(1-blend)+b.b*blend)*level));
}

uint8_t renderWitchfireSparkles(uint32_t elapsedMs) {
  static_assert(sparkleRegionPixels > 0 && sparkleFrameMs > 0 && sparkleOnMs > 0 &&
      sparkleRefreshMs >= 2*sparkleOnMs && sparkleRefreshMs % sparkleFrameMs == 0 &&
      sparkleColorShiftMs > 0 && sparkleThrobMs > 0 &&
      sparkleMinCount > 0 && sparkleMinCount <= sparkleMaxCount &&
      sparkleBrightnessPercent <= 100,
      "Sparkle settings must have positive periods and valid counts");
  const CRGB background = sparkleBackground(elapsedMs);
  for (uint16_t i=0; i<pixelCount; ++i) pixels[i] = background;

  for (uint32_t start=0; start<pixelCount; start+=sparkleRegionPixels) {
    const uint32_t seed = sparkleHash(start + 911U);
    const uint32_t offset = (seed % (sparkleRefreshMs / sparkleFrameMs)) * sparkleFrameMs;
    const uint64_t clock = uint64_t(elapsedMs) + offset;
    // The next frame rebuilds the background: no sparkle fade or retained trail.
    if (clock % sparkleRefreshMs >= sparkleOnMs) continue;
    uint32_t random = sparkleHash(seed + uint32_t(clock / sparkleRefreshMs));
    const uint16_t width = pixelCount-start < sparkleRegionPixels
        ? pixelCount-start : sparkleRegionPixels;
    uint8_t count = sparkleMinCount + random % (sparkleMaxCount-sparkleMinCount+1);
    if (count > width) count = width;
    uint16_t chosen[sparkleMaxCount];
    // Warm-white glints are distinct from the three-color background.
    const CRGB glint((230U * sparkleBrightnessPercent + 50) / 100,
                     (215U * sparkleBrightnessPercent + 50) / 100,
                     (185U * sparkleBrightnessPercent + 50) / 100);
    for (uint8_t n=0; n<count; ++n) {
      random = sparkleHash(random + n + 1);
      uint16_t candidate = random % width;
      // Resolve collisions so a requested three sparkles really means three pixels.
      bool duplicate;
      do {
        duplicate = false;
        for (uint8_t j=0; j<n; ++j) {
          if (candidate == chosen[j]) {
            candidate = (candidate + 1) % width;
            duplicate = true;
            break;
          }
        }
      } while (duplicate);
      chosen[n] = candidate;
      pixels[start + candidate] = glint;
    }
  }
  return outputBrightness;
}
