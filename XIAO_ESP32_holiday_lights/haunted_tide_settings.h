#pragma once
#include <stdint.h>

constexpr uint32_t hauntedTideFrameMs = 40;
constexpr uint8_t hauntedTideWaveMin = 20; // Palette brightness, out of 255.
constexpr uint8_t hauntedTideWaveMax = 75;
constexpr uint16_t fireflySpacingPixels = 10; // One independent firefly per region.
constexpr uint32_t fireflyMinGlowMs = 1800;
constexpr uint32_t fireflyGlowVariationMs = 1400;
constexpr uint32_t fireflyMinPauseMs = 700;
constexpr uint32_t fireflyPauseVariationMs = 1300;
