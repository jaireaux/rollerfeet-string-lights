#include "version.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "animation_clock.h"

// Hardware: XIAO D3 / GPIO4, WS2811 RGB, existing 300-pixel layout.
constexpr uint8_t ledDataPin = 4;
constexpr uint16_t pixelCount = 300;
constexpr uint8_t outputBrightness = 200; // Previous Classic Christmas setting.
constexpr uint16_t skippedPixelBegin = 210;
constexpr uint16_t skippedPixelEnd = pixelCount - 48; // Exclusive: 252.

// Scheduling and visual motion are separate settings.
constexpr uint32_t animationDurationMs = 15000;
constexpr uint32_t frameIntervalMs = 500;
constexpr uint32_t classicChristmasStepMs = 500;

CRGB pixels[pixelCount];
AnimationClock animationClock;

void renderClassicChristmas(uint32_t elapsedMs);
void applySkippedPixels();
void updateAnimation(uint32_t now);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, ledDataPin, RGB>(pixels, pixelCount)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(outputBrightness);
  animationClock.start(millis()); // Start after initialization, not before setup.
  Serial.println("Classic Christmas only - shared timing controller");
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
  renderClassicChristmas(elapsedMs);
  applySkippedPixels();
  FastLED.show(); // The only place a frame is sent to the lights.
}

void applySkippedPixels() {
  for (uint16_t i = skippedPixelBegin; i < skippedPixelEnd; ++i) {
    pixels[i] = CRGB::Black;
  }
}
