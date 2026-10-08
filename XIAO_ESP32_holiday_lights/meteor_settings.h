#pragma once
#include <stdint.h>

constexpr uint16_t meteorHeadPixels = 10;
constexpr uint32_t meteorSpeedPixelsPerSecond = 60;
constexpr uint32_t meteorTrailFadeMs = 800;
// Fixed time between meteor launches, independent of string length.
constexpr uint32_t meteorLaunchIntervalMs = 1250;
