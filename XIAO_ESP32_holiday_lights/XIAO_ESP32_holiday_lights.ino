#include "version.h"
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "animation_clock.h"
#include "meteor_settings.h"
#include "haunted_tide_settings.h"
#include "sparkle_settings.h"
#include "ota_service.h"
#include "light_control.h"
#include <stdio.h>
#include <string.h>

// Both profiles use the current 600-pixel string.
#ifndef HOLIDAY_LIGHTS_PRODUCTION
#define HOLIDAY_LIGHTS_PRODUCTION 0 // Set to 1 for the outdoor production display.
#endif
constexpr uint16_t developmentPixelCount = 600;
constexpr uint16_t productionPixelCount = 600;
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

// The web controls expose every completed effect without changing the preview default.
const Animation allAnimations[] = {
  {"Throb", 50, renderThrob},
  {"Orange / Purple", alternatingColorStepMs, renderAlternatingColors},
  {"Meteor Rain", 20, renderMeteorRain},
  {"Haunted Tide", hauntedTideFrameMs, renderHauntedTide},
  {"Witchfire Sparkles", sparkleFrameMs, renderWitchfireSparkles}
};
constexpr uint8_t allAnimationCount = sizeof(allAnimations) / sizeof(allAnimations[0]);
const Animation previewAnimations[] = {
  {"Meteor Rain", 20, renderMeteorRain},
  {"Witchfire Sparkles", sparkleFrameMs, renderWitchfireSparkles}
};
constexpr uint8_t previewAnimationCount = 2;
bool lightsEnabled = true, automaticCycling = true;
bool fullPlaylist = HOLIDAY_LIGHTS_PRODUCTION;
uint8_t brightnessPercent = 100, manualAnimationIndex = 2;
uint32_t runtimeDurationMs = animationDurationMs;

const Animation &selectedAnimation() {
  if (!automaticCycling) return allAnimations[manualAnimationIndex];
  return fullPlaylist ? allAnimations[currentAnimationIndex] : previewAnimations[currentAnimationIndex];
}
uint8_t selectedAnimationId() {
  const char *name = selectedAnimation().name;
  for (uint8_t i = 0; i < allAnimationCount; ++i)
    if (strcmp(name, allAnimations[i].name) == 0) return i;
  return 2;
}

// Commands are validated before any state changes. Values are bounded at the firmware.
bool applyLightCommand(const char *action, uint32_t value, uint32_t now) {
  if (!action || networkUpdateBusy()) return false;
  if (strcmp(action, "power") == 0 && value <= 1) lightsEnabled = value;
  else if (strcmp(action, "brightness") == 0 && value <= 100) brightnessPercent = value;
  else if (strcmp(action, "animation") == 0 && value < allAnimationCount) {
    manualAnimationIndex = value;
    fullPlaylist = true;
    currentAnimationIndex = value;
    automaticCycling = true;
    animationClock.start(now);
  } else if (strcmp(action, "auto") == 0 && value <= 1) {
    const uint8_t previous = selectedAnimationId();
    automaticCycling = value;
    manualAnimationIndex = previous;
    currentAnimationIndex = fullPlaylist ? previous : 0;
    if (!fullPlaylist) {
      for (uint8_t i = 0; i < previewAnimationCount; ++i)
        if (strcmp(previewAnimations[i].name, allAnimations[previous].name) == 0) currentAnimationIndex = i;
    }
    animationClock.start(now);
  } else if (strcmp(action, "playlist") == 0 && value <= 1) {
    fullPlaylist = value;
    automaticCycling = true;
    currentAnimationIndex = 0;
    animationClock.start(now);
  } else if (strcmp(action, "duration") == 0 && value >= 10 && value <= 600) {
    runtimeDurationMs = value * 1000;
    animationClock.start(now);
  } else if (strcmp(action, "next") == 0 && value == 1) {
    if (automaticCycling) currentAnimationIndex = (currentAnimationIndex + 1) %
        (fullPlaylist ? allAnimationCount : previewAnimationCount);
    else manualAnimationIndex = (manualAnimationIndex + 1) % allAnimationCount;
    animationClock.start(now);
  } else return false;
  updateAnimation(now);
  sendCurrentFrame(now); // Brightness/power must respond even between slow effect frames.
  return true;
}

void writeLightState(char *buffer, size_t capacity) {
  const uint32_t elapsed = millis() - animationClock.animationStartedAtMs;
  const uint32_t remaining = automaticCycling && elapsed < runtimeDurationMs ? runtimeDurationMs - elapsed : 0;
  snprintf(buffer, capacity,
      "{\"version\":\"" HOLIDAY_LIGHTS_VERSION "\",\"pixels\":%u,"
      "\"power\":%s,\"brightness\":%u,\"auto\":%s,\"playlist\":\"%s\","
      "\"duration\":%lu,\"animation\":%u,\"updating\":%s,\"remaining_ms\":%lu}",
      unsigned(pixelCount), lightsEnabled ? "true" : "false", unsigned(brightnessPercent),
      automaticCycling ? "true" : "false", fullPlaylist ? "all" : "preview",
      static_cast<unsigned long>(runtimeDurationMs / 1000), unsigned(selectedAnimationId()),
      networkUpdateBusy() ? "true" : "false", static_cast<unsigned long>(remaining));
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, ledDataPin, RGB>(displayedPixels, pixelCount)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(outputBrightness);
  animationClock.start(millis()); // Start after initialization, not before setup.
  Serial.println("Halloween playlist starting:");
  Serial.println(selectedAnimation().name);
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
  if (automaticCycling && animationClock.finished(now, runtimeDurationMs)) {
    currentAnimationIndex = (currentAnimationIndex + 1) % (fullPlaylist ? allAnimationCount : previewAnimationCount);
    animationClock.start(now);
    Serial.println(selectedAnimation().name);
  }
  const Animation &animation = selectedAnimation();
  uint32_t elapsedMs;
  if (!animationClock.frameDue(now, animation.frameIntervalMs, elapsedMs)) {
    return;
  }
  animationBrightness = animation.render(elapsedMs);
  // applySkippedPixels(); // Blackout disabled until the outdoor layout is finalized.
  sendCurrentFrame(now);
}

void sendCurrentFrame(uint32_t now) {
  // Keep the animation buffer intact. Scale its brightness before adding status,
  // so a low Throb brightness cannot make the indicator unreadably dim.
  for (uint16_t i = 0; i < pixelCount; ++i) {
    displayedPixels[i] = CRGB(uint32_t(pixels[i].r) * animationBrightness * (lightsEnabled ? brightnessPercent : 0) / (outputBrightness * 100u),
                             uint32_t(pixels[i].g) * animationBrightness * (lightsEnabled ? brightnessPercent : 0) / (outputBrightness * 100u),
                             uint32_t(pixels[i].b) * animationBrightness * (lightsEnabled ? brightnessPercent : 0) / (outputBrightness * 100u));
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
