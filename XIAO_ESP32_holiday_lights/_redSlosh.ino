#include <math.h>
#include "spatial_map.h"
#include "red_slosh_timing.h"

// SECTION 1 — RED BACKGROUND (bottom-left quadrant pulse for phase one).
// A continuous field over physical x/y: neighbors share a similar brightness.
// Wave clocks are independent of the 22-second shadow choreography.
float redSloshLevel(uint32_t elapsedMs, float x, float y) {
  constexpr float tau = 6.28318530718f;
  const float phaseA = tau * float(elapsedMs % 24000) / 24000.0f;
  const float phaseB = tau * float(elapsedMs % 37000) / 37000.0f;
  const float wave = 0.7f*sinf(tau*(0.45f*x+0.12f*y)-phaseA)
                   +0.3f*sinf(tau*(-0.20f*x+0.30f*y)+phaseB);
  return 0.875f+0.125f*wave; // 75–100% before shadow.
}

bool redSloshBottomLeft(uint16_t i) {
  return i < spatialPixelCount && spatialMap[i].x < 32768 && spatialMap[i].y < 32768;
}

float redSloshQuadrantLevel(uint32_t elapsedMs) {
  constexpr float tau=6.28318530718f;
  return 0.875f-0.125f*cosf(tau*float(elapsedMs%6000)/6000.0f);
}

void renderRedSloshBackground(uint32_t elapsedMs) {
  const float level=redSloshQuadrantLevel(elapsedMs);
  const CRGB red(uint8_t(255*level),0,uint8_t(4*level));
  for (uint16_t i=0; i<pixelCount; ++i) {
    pixels[i]=redSloshBottomLeft(i) ? red : CRGB(CRGB::Black);
  }
}

// SECTION 2 — SHADOW MASK (preserved for phase two; call disabled below).
void applyRedSloshShadow(uint32_t elapsedMs) {
  const float shadow=redShadowCenter(elapsedMs);
  for (uint16_t i=0; i<spatialPixelCount && i<pixelCount; ++i) {
    const float distance=fabsf(spatialMap[i].x/65535.0f-shadow);
    if (distance>=0.10f) continue; // Uncovered background stays exactly intact.
    if (distance<=0.07f) {pixels[i]=CRGB::Black;continue;}
    const float edge=(distance-0.07f)/0.03f;
    const float light=edge*edge*(3-2*edge);
    pixels[i]=CRGB(uint8_t(pixels[i].r*light),0,uint8_t(pixels[i].b*light));
  }
}

uint8_t renderRedSlosh(uint32_t elapsedMs) {
  renderRedSloshBackground(elapsedMs); // Shared quadrant pulse; all other positions dark.
  // applyRedSloshShadow(elapsedMs); // Disabled: isolate the red background first.
  return outputBrightness;
}
