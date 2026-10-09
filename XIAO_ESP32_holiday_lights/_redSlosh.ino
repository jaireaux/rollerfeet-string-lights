#include <math.h>
#include "spatial_map.h"
#include "red_slosh_timing.h"
#include "throb_envelope.h"

// SECTION 1 — QUADRANT BACKGROUND (leftward scrolling red pulses for phase one).
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

bool redSloshTopLeft(uint16_t i) {
  return i < spatialPixelCount && spatialMap[i].x < 32768 && spatialMap[i].y >= 32768;
}

bool redSloshTopRight(uint16_t i) {
  return i < spatialPixelCount && spatialMap[i].x >= 32768 && spatialMap[i].y >= 32768;
}

bool redSloshBottomRight(uint16_t i) {
  return i < spatialPixelCount && spatialMap[i].x >= 32768 && spatialMap[i].y < 32768;
}

constexpr uint8_t redSloshMinBrightness=uint32_t(outputBrightness)*30/100;

// Delay each quadrant's full Throb envelope by a quarter-cycle beat.
uint8_t redSloshQuadrantBrightness(uint32_t elapsedMs, uint8_t quadrant) {
  const uint32_t phase=(elapsedMs%throbPeriodMs+throbPeriodMs-
                        quadrant*(throbPeriodMs/4))%throbPeriodMs;
  return throbBrightness(phase,throbPeriodMs,redSloshMinBrightness,outputBrightness);
}

// One full-height down/up sine oscillation per two seconds.
uint32_t redSloshVerticalShift(uint32_t elapsedMs) {
  constexpr float tau=6.28318530718f;
  const float phase=tau*float(elapsedMs%2000)/2000.0f;
  return uint32_t(lroundf(32768.0f*(1.0f-cosf(phase))));
}

// Inverse rotation around the original map center makes the displayed field
// rotate clockwise. Translation is sampled afterward, so the whole moving field rotates.
SpatialPoint redSloshRotatePoint(SpatialPoint point, float cosine, float sine) {
  const float x=float(point.x)-32768.0f, y=float(point.y)-32768.0f;
  return {uint16_t(int32_t(lroundf(32768.0f+x*cosine-y*sine))),
          uint16_t(int32_t(lroundf(32768.0f+x*sine+y*cosine)))};
}

uint8_t redSloshPatternQuadrant(uint16_t i, uint32_t xShift, uint32_t yShift,
                               float cosine, float sine) {
  const SpatialPoint rotated=redSloshRotatePoint(spatialMap[i],cosine,sine);
  const uint16_t x=uint16_t(uint32_t(rotated.x)+xShift);
  const uint16_t y=uint16_t(uint32_t(rotated.y)+yShift);
  const bool left=x<32768, bottom=y<32768;
  return bottom ? (left?0:3) : (left?1:2);
}

uint8_t redSloshMovingQuadrant(uint16_t i, uint32_t elapsedMs) {
  const uint32_t xShift=(elapsedMs%6000)*65536u/6000;
  const float angle=6.28318530718f*float(elapsedMs%1000)/1000.0f;
  return redSloshPatternQuadrant(i,xShift,redSloshVerticalShift(elapsedMs),
                                cosf(angle),sinf(angle));
}

void renderRedSloshBackground(uint32_t elapsedMs) {
  const CRGB colors[]={CRGB(255,0,4),CRGB(255,0,4),CRGB(255,0,4),CRGB(255,0,4)};
  CRGB levels[4];
  for(uint8_t q=0;q<4;++q) {
    const uint8_t brightness=redSloshQuadrantBrightness(elapsedMs,q);
    levels[q]=CRGB(uint32_t(colors[q].r)*brightness/outputBrightness,
                   uint32_t(colors[q].g)*brightness/outputBrightness,
                   uint32_t(colors[q].b)*brightness/outputBrightness);
  }
  const uint32_t xShift=(elapsedMs%6000)*65536u/6000;
  const uint32_t yShift=redSloshVerticalShift(elapsedMs); // Compute sine once per frame.
  const float angle=6.28318530718f*float(elapsedMs%1000)/1000.0f;
  const float cosine=cosf(angle), sine=sinf(angle); // Once per frame.
  for(uint16_t i=0;i<pixelCount;++i) {
    pixels[i]=i<spatialPixelCount ? levels[redSloshPatternQuadrant(i,xShift,yShift,cosine,sine)]
                                  : CRGB(CRGB::Black);
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
  renderRedSloshBackground(elapsedMs); // Quarter-cycle offsets: red, purple, green, orange.
  // applyRedSloshShadow(elapsedMs); // Disabled: isolate the red background first.
  return outputBrightness; // Per-quadrant envelopes are already applied above.
}
