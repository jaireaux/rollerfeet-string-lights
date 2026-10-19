#pragma once
#include <math.h>
#include <stdint.h>

// Target: https://github.com/jaireaux/rollerfeet-string-lights
// Adapt for other firmware/hardware before use. Numeric frame logic translated
// from the operator's simulator source, retained in docs/simulator-sources/.
constexpr float brewTau = 6.28318530718f;
struct BrewRgb { float r, g, b; };
struct BrewBubble { float progress, cx, cy, radius; };
struct BrewFrame { float simmer, steamPhase; BrewBubble bubbles[3]; };

inline float brewSoft(float distance, float width) {
  float a = 1.0f - distance / width;
  if (a < 0) return 0;
  if (a > 1) return 1;
  return a * a * (3 - 2 * a);
}

// One boundary shared by the liquid and bubble destinations.
inline float brewSurfaceAt(float x, float simmer) {
  return 0.85f + 0.04f * sinf(brewTau * x + simmer);
}
inline float brewBubbleCenterY(float progress, float surface, float radius) {
  constexpr float startY = 0.04f;
  return startY + (surface - radius - startY) * progress;
}
inline BrewFrame brewFrameAt(uint32_t elapsedMs) {
  BrewFrame frame;
  frame.simmer = brewTau * float(elapsedMs % 15000) / 15000.0f;
  frame.steamPhase = brewTau * float(elapsedMs % 9000) / 9000.0f;
  for (uint8_t b = 0; b < 3; ++b) {
    const uint32_t periodMs = 6200 + 1100 * b;
    // Widen before offset addition; keep phases accurate at large uptimes.
    const float progress = float((uint64_t(elapsedMs) + uint32_t(b) * 1700) % periodMs) / periodMs;
    const float cx = 0.18f + 0.30f * b + 0.07f * sinf(frame.simmer + b);
    const float radius = 0.025f + 0.07f * progress;
    const float surface = brewSurfaceAt(cx, frame.simmer);
    frame.bubbles[b] = {progress, cx, brewBubbleCenterY(progress, surface, radius), radius};
  }
  return frame;
}

inline BrewRgb brewRenderBackground(float x, float y, const BrewFrame &frame) {
  const float surface = brewSurfaceAt(x, frame.simmer);
  const float pool = brewSoft(y - surface, 0.07f);
  BrewRgb rgb = {0, (35 + 25 * (0.5f + 0.5f * sinf(brewTau * (1.4f * x + y) - frame.simmer))) * pool, 0};
  for (uint8_t b = 0; b < 3; ++b) {
    const BrewBubble &bubble = frame.bubbles[b];
    const float dx = x - bubble.cx, dy = y - bubble.cy;
    const float ring = brewSoft(fabsf(sqrtf(dx * dx + dy * dy) - bubble.radius), 0.055f);
    rgb.r += 105 * ring;
    rgb.b += 140 * ring;
    if (bubble.progress > 0.95f) {
      const float spark = brewSoft(fabsf(x - bubble.cx), 0.14f)
                        * brewSoft(fabsf(y - surface), 0.10f);
      rgb.r += 200 * spark;
      rgb.g += 65 * spark;
    }
  }
  rgb.r = fminf(rgb.r, 255); rgb.g = fminf(rgb.g, 255); rgb.b = fminf(rgb.b, 255);
  return rgb;
}

inline BrewRgb brewRenderForeground(float x, float y, const BrewFrame &frame, BrewRgb rgb) {
  if (y <= 2.0f / 3) return rgb;
  const float height = (y - 2.0f / 3) * 3;
  const float center = 0.5f + 0.13f * sinf(frame.steamPhase - 3 * height);
  const float wisp = brewSoft(fabsf(x - center), 0.30f)
                   + 0.6f * brewSoft(fabsf(x - (center + 0.26f)), 0.195f);
  const float white = 255 * (0.60f + 0.05f * sinf(frame.steamPhase))
                    * fminf(wisp, 1) * brewSoft(1 - height, 1);
  rgb.r = fminf(255, rgb.r + white);
  rgb.g = fminf(255, rgb.g + white);
  rgb.b = fminf(255, rgb.b + white);
  return rgb;
}
inline uint8_t brewChannel(float value) {
  return isfinite(value) ? uint8_t(lroundf(fmaxf(0, fminf(255, value)))) : 0;
}
