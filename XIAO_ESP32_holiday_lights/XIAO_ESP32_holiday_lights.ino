#include "version.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "animation_clock.h"
#include "meteor_settings.h"
#include "haunted_tide_settings.h"
#include "sparkle_settings.h"

// Development uses the current 100-pixel test string.
#ifndef HOLIDAY_LIGHTS_PRODUCTION
#define HOLIDAY_LIGHTS_PRODUCTION 0 // Set to 1 for the outdoor production display.
#endif
constexpr uint16_t developmentPixelCount = 100;
constexpr uint16_t productionPixelCount = 300; // Previous layout; confirm before deployment.
constexpr uint16_t pixelCount = HOLIDAY_LIGHTS_PRODUCTION
    ? productionPixelCount : developmentPixelCount;

// Hardware: XIAO D3 / GPIO4, WS2811 RGB.
constexpr uint8_t ledDataPin = 4;
constexpr uint8_t outputBrightness = 200; // Upper brightness limit for the effect.
constexpr uint16_t skippedPixelBegin = 210;
constexpr uint16_t skippedPixelEnd = 252; // Exclusive; fixed physical connecting section.

// One runtime for every animation; effect speeds remain independent.
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
uint8_t renderHauntedTide(uint32_t elapsedMs);
uint8_t renderWitchfireSparkles(uint32_t elapsedMs);
void applySkippedPixels();
void updateAnimation(uint32_t now);

struct Animation {
  const char *name;
  uint32_t frameIntervalMs;
  uint8_t (*render)(uint32_t elapsedMs);
};

// Development previews two selected effects (currently Meteor Rain + Witchfire Sparkles).
// Update this pair as requested; retain the full production list.
const Animation animations[] = {
#if HOLIDAY_LIGHTS_PRODUCTION
  {"Throb", 50, renderThrob},
  {"Orange / Purple", alternatingColorStepMs, renderAlternatingColors},
  {"Meteor Rain", 20, renderMeteorRain},
  {"Haunted Tide", hauntedTideFrameMs, renderHauntedTide},
#else
  {"Meteor Rain", 20, renderMeteorRain},
#endif
  {"Witchfire Sparkles", sparkleFrameMs, renderWitchfireSparkles}
};
constexpr uint8_t animationCount = sizeof(animations) / sizeof(animations[0]);
uint8_t currentAnimationIndex = 0;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, ledDataPin, RGB>(pixels, pixelCount)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(outputBrightness);
  animationClock.start(millis()); // Start after initialization, not before setup.
  Serial.println("Halloween playlist starting:");
  Serial.println(animations[currentAnimationIndex].name);
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
  for (uint16_t i = skippedPixelBegin; i < skippedPixelEnd && i < pixelCount; ++i) {
    pixels[i] = CRGB::Black;
  }
}
