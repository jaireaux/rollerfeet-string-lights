#include "version.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "animation_clock.h"

// Hardware: XIAO D3 / GPIO4, WS2811 RGB, existing 300-pixel layout.
constexpr uint8_t ledDataPin = 4;
constexpr uint16_t pixelCount = 300;
constexpr uint8_t outputBrightness = 200; // Upper brightness limit for the effect.
constexpr uint16_t skippedPixelBegin = 210;
constexpr uint16_t skippedPixelEnd = pixelCount - 48; // Exclusive: 252.

// Scheduling and visual motion are separate settings.
constexpr uint32_t frameIntervalMs = 50;
constexpr uint32_t throbPeriodMs = 3000;
constexpr uint32_t animationDurationMs = 3 * throbPeriodMs; // Full color sequence.
constexpr uint8_t throbMinBrightness = 10;

CRGB pixels[pixelCount];
AnimationClock animationClock;

uint8_t renderThrob(uint32_t elapsedMs);
void applySkippedPixels();
void updateAnimation(uint32_t now);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, ledDataPin, RGB>(pixels, pixelCount)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(outputBrightness);
  animationClock.start(millis()); // Start after initialization, not before setup.
  Serial.println("Halloween Throb - orange/purple/green - shared timing controller");
}

void loop() {
  updateAnimation(millis());
  // Future control and OTA services can run here on every pass.
}

void updateAnimation(uint32_t now) {
  uint32_t elapsedMs;
  if (!animationClock.frameDue(now, animationDurationMs, frameIntervalMs,
                               elapsedMs)) {
    return;
  }
  FastLED.setBrightness(renderThrob(elapsedMs));
  applySkippedPixels();
  FastLED.show(); // The only place a frame is sent to the lights.
}

void applySkippedPixels() {
  for (uint16_t i = skippedPixelBegin; i < skippedPixelEnd; ++i) {
    pixels[i] = CRGB::Black;
  }
}
