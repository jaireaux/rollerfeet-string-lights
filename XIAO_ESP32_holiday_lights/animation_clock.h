#pragma once
#include <stdint.h>

// Shared scheduling state. All times are milliseconds, using unsigned
// subtraction so a millis() rollover does not interrupt the animation.
struct AnimationClock {
  uint32_t animationStartedAtMs = 0;
  uint32_t lastFrameAtMs = 0;
  bool firstFramePending = true;

  void start(uint32_t now) {
    animationStartedAtMs = now;
    lastFrameAtMs = now;
    firstFramePending = true;
  }

  bool finished(uint32_t now, uint32_t durationMs) const {
    return now - animationStartedAtMs >= durationMs;
  }

  bool frameDue(uint32_t now, uint32_t frameIntervalMs, uint32_t &elapsedMs) {
    elapsedMs = now - animationStartedAtMs;
    if (!firstFramePending && now - lastFrameAtMs < frameIntervalMs) {
      return false;
    }
    firstFramePending = false;
    lastFrameAtMs = now;
    return true; // One current frame; never burst through missed frames.
  }
};
