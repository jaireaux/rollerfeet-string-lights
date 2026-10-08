#include <math.h>

// Pacifica-inspired layered waves with independent, gently pulsing fireflies.
// Every frame is reconstructed from animation elapsed time; no retained state.
uint32_t tideHash(uint32_t value) {
  value ^= value >> 16;
  value *= 0x7feb352dU;
  value ^= value >> 15;
  value *= 0x846ca68bU;
  return value ^ (value >> 16);
}

float tideWave(uint32_t elapsedMs, uint32_t periodMs, float position) {
  constexpr float tau = 6.28318530718f;
  return 0.5f + 0.5f * sinf(tau * (float(elapsedMs % periodMs) / periodMs + position));
}

uint8_t renderHauntedTide(uint32_t elapsedMs) {
  static_assert(fireflySpacingPixels > 0, "Firefly spacing must be positive");
  static const CRGB colors[] = {
    CRGB(128, 70, 0), CRGB(64, 0, 64), CRGB(0, 50, 0)
  };
  for (uint16_t i = 0; i < pixelCount; ++i) {
    // Different wavelengths and directions make broad pools of color drift.
    const float a = tideWave(elapsedMs, 17000, float(i) / 43);
    const float b = tideWave(elapsedMs, 23000, -float(i) / 67);
    const float c = tideWave(elapsedMs, 31000, float(i) / 29);
    const float d = tideWave(elapsedMs, 13000, -float(i) / 97);
    const float colorPosition = 2.999f * (0.45f*a + 0.35f*b + 0.20f*c);
    const uint8_t first = uint8_t(colorPosition);
    const float blend = colorPosition - first;
    const CRGB &left = colors[first];
    const CRGB &right = colors[(first + 1) % 3];
    // Wave alignment creates soft highlights rather than white flashes.
    const float level = (hauntedTideWaveMin + (hauntedTideWaveMax - hauntedTideWaveMin)
                        * (0.55f*c + 0.45f*d)) / 255.0f;
    pixels[i] = CRGB(uint8_t((left.r*(1-blend) + right.r*blend)*level),
                     uint8_t((left.g*(1-blend) + right.g*blend)*level),
                     uint8_t((left.b*(1-blend) + right.b*blend)*level));
  }

  for (uint32_t start = 0; start < pixelCount; start += fireflySpacingPixels) {
    const uint32_t seed = tideHash(start + 173U);
    const uint32_t glowMs = fireflyMinGlowMs + seed % (fireflyGlowVariationMs + 1);
    const uint32_t periodMs = glowMs + fireflyMinPauseMs
        + (seed >> 12) % (fireflyPauseVariationMs + 1);
    const uint64_t clock = uint64_t(elapsedMs) + seed % periodMs;
    const uint32_t phase = clock % periodMs;
    if (phase >= glowMs) continue;
    const uint32_t cycle = uint32_t(clock / periodMs);
    const uint32_t choice = tideHash(seed + cycle);
    const uint16_t remaining = pixelCount - start;
    const uint16_t width = remaining < fireflySpacingPixels ? remaining : fireflySpacingPixels;
    const uint16_t index = start + choice % width;
    // A squared sine eases in/out; position changes only while extinguished.
    const float pulse = sinf(3.14159265359f * float(phase) / glowMs);
    const float glow = pulse * pulse;
    const CRGB &color = colors[(choice >> 16) % 3];
    const CRGB background = pixels[index];
    pixels[index] = CRGB(uint8_t(background.r*(1-glow) + color.r*glow),
                         uint8_t(background.g*(1-glow) + color.g*glow),
                         uint8_t(background.b*(1-glow) + color.b*glow));
  }
  return outputBrightness;
}
