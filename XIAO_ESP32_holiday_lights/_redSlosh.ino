#include <math.h>
#include "spatial_map.h"
#include "red_slosh_timing.h"

// Two broad spatial waves move in opposite directions, independent of string order.
uint8_t renderRedSlosh(uint32_t elapsedMs) {
  constexpr float tau = 6.28318530718f;
  const float shadow = redShadowCenter(elapsedMs);
  const float phaseA = tau * float(elapsedMs % 5400) / 5400.0f;
  const float phaseB = tau * float(elapsedMs % 7900) / 7900.0f;
  for (uint16_t i=0; i<pixelCount; ++i) {
    if (i >= spatialPixelCount) { pixels[i]=CRGB::Black; continue; }
    const float x=spatialMap[i].x/65535.0f, y=spatialMap[i].y/65535.0f;
    const float wave=0.6f*sinf(tau*(0.8f*x+0.25f*y)-phaseA)
                    +0.4f*sinf(tau*(-0.55f*x+0.7f*y)+phaseB);
    const float level=0.875f+0.125f*wave; // 75–100% of deep red before shadow.
    const float distance=fabsf(x-shadow);
    // Total band width 20%; central 14% dark, outer 3% on each side soft.
    float edge=(distance-0.07f)/0.03f;
    if (edge<0) edge=0; if (edge>1) edge=1;
    const float light=level*edge*edge*(3-2*edge);
    pixels[i]=CRGB(uint8_t(255*light), 0, uint8_t(4*light));
  }
  return outputBrightness;
}
