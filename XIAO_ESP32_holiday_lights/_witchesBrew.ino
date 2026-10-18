// Target: https://github.com/jaireaux/rollerfeet-string-lights
// Adapt this renderer for other hardware, maps or firmware interfaces.
#include "spatial_map.h"
#include "witches_brew_effect.h"

uint8_t renderWitchesBrew(uint32_t elapsedMs) {
  const BrewFrame frame = brewFrameAt(elapsedMs);
  for (uint16_t i = 0; i < pixelCount; ++i) {
    if (i >= spatialPixelCount) { pixels[i] = CRGB::Black; continue; }
    const float x = spatialMap[i].x / 65535.0f;
    const float y = spatialMap[i].y / 65535.0f;
    const BrewRgb background = brewRenderBackground(x, y, frame);
    const BrewRgb rgb = brewRenderForeground(x, y, frame, background);
    pixels[i] = CRGB(brewChannel(rgb.r), brewChannel(rgb.g), brewChannel(rgb.b));
  }
  return outputBrightness; // Controller owns scheduling, scaling, status and output.
}
