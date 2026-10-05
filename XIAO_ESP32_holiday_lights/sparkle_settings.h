#pragma once
#include <stdint.h>

constexpr uint32_t sparkleFrameMs = 30;
constexpr uint32_t sparkleColorShiftMs = 6000; // Orange -> purple -> green -> orange.
constexpr uint32_t sparkleThrobMs = 3000;
constexpr uint8_t sparkleBackgroundMin = 35;
constexpr uint8_t sparkleBackgroundMax = 130;
constexpr uint16_t sparkleRegionPixels = 25;
constexpr uint32_t sparkleRefreshMs = 240; // Each region fires once per cycle, with staggered regions.
// A sparkle is visible for one nominal frame, then returns to the background.
constexpr uint32_t sparkleOnMs = sparkleFrameMs;
constexpr uint8_t sparkleMinCount = 1;
constexpr uint8_t sparkleMaxCount = 3;
