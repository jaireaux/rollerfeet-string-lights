#pragma once
#include <stdint.h>
constexpr uint32_t redSloshCycleMs = 22000;
// Shadow center: -0.1 and 1.1 keep the 20%-wide band entirely off-scene.
inline float redShadowCenter(uint32_t elapsedMs) {
  const uint32_t t = elapsedMs % redSloshCycleMs;
  if (t < 7000) return -0.1f;
  if (t < 8000) return -0.1f + 1.2f*(t-7000)/1000.0f;
  if (t < 11000) return 1.1f;
  if (t < 12000) return 1.1f - 1.2f*(t-11000)/1000.0f;
  if (t < 15000) return -0.1f;
  if (t < 15500) return -0.1f + 0.6f*(t-15000)/500.0f;
  if (t < 16500) return 0.5f;
  if (t < 17000) return 0.5f - 0.6f*(t-16500)/500.0f;
  if (t < 20000) return -0.1f;
  if (t < 20500) return -0.1f + 0.6f*(t-20000)/500.0f;
  if (t < 21500) return 0.5f;
  return 0.5f + 0.6f*(t-21500)/500.0f;
}
