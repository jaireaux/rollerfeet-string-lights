#include "version.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "animation_clock.h"
#include "meteor_settings.h"

// Hardware: XIAO D3 / GPIO4, WS2811 RGB, existing 300-pixel layout.
constexpr uint8_t ledDataPin = 4;
constexpr uint16_t pixelCount = 300;
constexpr uint8_t outputBrightness = 200; // Upper brightness limit for the effect.
constexpr uint16_t skippedPixelBegin = 210;
constexpr uint16_t skippedPixelEnd = pixelCount - 48; // Exclusive: 252.

// One runtime for every animation; effect speeds remain independent.
#ifndef HOLIDAY_LIGHTS_PRODUCTION
#define HOLIDAY_LIGHTS_PRODUCTION 0 // Set to 1 for the outdoor production display.
#endif
constexpr uint32_t animationDurationMs = HOLIDAY_LIGHTS_PRODUCTION ? 180000 : 30000;

// Scheduling and visual motion are separate settings.
constexpr uint32_t throbPeriodMs = 3000;
constexpr uint32_t alternatingColorStepMs = 1000;
constexpr uint8_t throbMinBrightness = 10;

CRGB pixels[pixelCount];
AnimationClock animationClock;

uint8_t renderThrob(uint32_t elapsedMs);
uint8_t renderAlternatingColors(uint32_t elapsedMs);
uint8_t renderMeteorRain(uint32_t elapsedMs);
void applySkippedPixels();
void updateAnimation(uint32_t now);

struct Animation {
  const char *name;
  uint32_t frameIntervalMs;
  uint8_t (*render)(uint32_t elapsedMs);
};

const Animation animations[] = {
  {"Throb", 50, renderThrob},
  {"Orange / Purple", alternatingColorStepMs, renderAlternatingColors},
  {"Meteor Rain", 20, renderMeteorRain}
};
constexpr uint8_t animationCount = sizeof(animations) / sizeof(animations[0]);
uint8_t currentAnimationIndex = 0;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, ledDataPin, RGB>(pixels, pixelCount)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(outputBrightness);
  animationClock.start(millis()); // Start after initialization, not before setup.
  Serial.println("Halloween playlist: Throb, Orange / Purple, Meteor Rain");
}

void loop() {
  updateAnimation(millis());
  // Future control and OTA services can run here on every pass.
}

void updateAnimation(uint32_t now) {
  if (animationClock.finished(now, animationDurationMs)) {
    currentAnimationIndex = (currentAnimationIndex + 1) % animationCount;
    animationClock.start(now);
    Serial.println(animations[currentAnimationIndex].name);
  }
  const Animation &animation = animations[currentAnimationIndex];
  uint32_t elapsedMs;
  if (!animationClock.frameDue(now, animation.frameIntervalMs, elapsedMs)) {
    return;
  }
  FastLED.setBrightness(animation.render(elapsedMs));
  applySkippedPixels();
  FastLED.show(); // The only place a frame is sent to the lights.
}

void applySkippedPixels() {
  for (uint16_t i = skippedPixelBegin; i < skippedPixelEnd; ++i) {
    pixels[i] = CRGB::Black;
  }
}
