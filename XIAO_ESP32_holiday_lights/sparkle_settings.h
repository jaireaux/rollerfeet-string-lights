#pragma once
#include <stdint.h>

constexpr uint32_t sparkleFrameMs = 30;
constexpr uint32_t sparkleColorShiftMs = 6000; // Orange -> purple -> green -> orange.
constexpr uint32_t sparkleThrobMs = 3000;
constexpr uint8_t sparkleBackgroundMin = 35;
constexpr uint8_t sparkleBackgroundMax = 130;
constexpr uint16_t sparkleRegionPixels = 25;
constexpr uint32_t sparkleRefreshMs = 240; // Short flashes, with staggered regions.
constexpr uint8_t sparkleMinCount = 1;
constexpr uint8_t sparkleMaxCount = 3;
