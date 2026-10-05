#pragma once
#include <stdint.h>

constexpr uint16_t meteorHeadPixels = 10;
constexpr uint32_t meteorSpeedPixelsPerSecond = 60;
constexpr uint32_t meteorTrailFadeMs = 800;
constexpr uint32_t meteorLaunchPauseMs = 700;

// Launch two meteors during the former single-meteor cycle.
constexpr uint8_t meteorsPerCycle = 2;
