#include "version.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "animation_clock.h"
#include "meteor_settings.h"
#include "haunted_tide_settings.h"
#include "sparkle_settings.h"
#include "ota_service.h"

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
static_assert(pixelCount > 0 && outputBrightness > 0, "Status output needs pixels and brightness");
constexpr uint16_t skippedPixelBegin = 210;
constexpr uint16_t skippedPixelEnd = 252; // Exclusive; fixed physical connecting section.

// One runtime for every animation; effect speeds remain independent.
constexpr uint32_t animationDurationMs = HOLIDAY_LIGHTS_PRODUCTION ? 180000 : 30000;

// Scheduling and visual motion are separate settings.
constexpr uint32_t throbPeriodMs = 3000;
constexpr uint32_t alternatingColorStepMs = 1000;
constexpr uint8_t throbMinBrightness = 10;

CRGB pixels[pixelCount];
CRGB displayedPixels[pixelCount];
uint8_t animationBrightness = outputBrightness;
StatusPixel displayedStatus = {false, 0, 0, 0};
AnimationClock animationClock;

uint8_t renderThrob(uint32_t elapsedMs);
uint8_t renderAlternatingColors(uint32_t elapsedMs);
uint8_t renderMeteorRain(uint32_t elapsedMs);
uint8_t renderHauntedTide(uint32_t elapsedMs);
uint8_t renderWitchfireSparkles(uint32_t elapsedMs);
void applySkippedPixels();
void updateAnimation(uint32_t now);
void sendCurrentFrame(uint32_t now);
void refreshStatusFrame(uint32_t now);

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
  FastLED.addLeds<WS2811, ledDataPin, RGB>(displayedPixels, pixelCount)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(outputBrightness);
  animationClock.start(millis()); // Start after initialization, not before setup.
  Serial.println("Halloween playlist starting:");
  Serial.println(animations[currentAnimationIndex].name);
  setStatusFrameCallback(refreshStatusFrame);
  beginNetworkUpdates();
}

void loop() {
  serviceNetworkUpdates(millis());
  const uint32_t now = millis(); // Network servicing may have taken time.
  if (takeAnimationRestartRequest()) animationClock.start(now);
  if (!networkUpdateBusy()) updateAnimation(now);
  refreshStatusFrame(now); // Status changes are independent of animation cadence.
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
  animationBrightness = animation.render(elapsedMs);
  applySkippedPixels();
  sendCurrentFrame(now);
}

void sendCurrentFrame(uint32_t now) {
  // Keep the animation buffer intact. Scale its brightness before adding status,
  // so a low Throb brightness cannot make the indicator unreadably dim.
  for (uint16_t i = 0; i < pixelCount; ++i) {
    displayedPixels[i] = CRGB(uint16_t(pixels[i].r) * animationBrightness / outputBrightness,
                             uint16_t(pixels[i].g) * animationBrightness / outputBrightness,
                             uint16_t(pixels[i].b) * animationBrightness / outputBrightness);
  }
  displayedStatus = networkStatusPixel(now);
  if (displayedStatus.active) {
    displayedPixels[0] = CRGB(displayedStatus.r, displayedStatus.g, displayedStatus.b);
  }
  FastLED.setBrightness(outputBrightness);
  FastLED.show(); // One output path for animation frames and status callbacks.
}

void refreshStatusFrame(uint32_t now) {
  const StatusPixel status = networkStatusPixel(now);
  if (status.active != displayedStatus.active ||
      (status.active && (status.r != displayedStatus.r || status.g != displayedStatus.g ||
                        status.b != displayedStatus.b))) sendCurrentFrame(now);
}

void applySkippedPixels() {
  for (uint16_t i = skippedPixelBegin; i < skippedPixelEnd && i < pixelCount; ++i) {
    pixels[i] = CRGB::Black;
  }
}
