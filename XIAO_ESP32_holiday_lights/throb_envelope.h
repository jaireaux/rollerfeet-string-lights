#pragma once
#include <stdint.h>
// Shared original Throb ramp: one complete rise/fall per period.
inline uint8_t throbBrightness(uint32_t elapsedMs, uint32_t periodMs,
                               uint8_t minimum, uint8_t maximum) {
  const uint32_t halfPeriodMs=periodMs/2;
  const uint32_t phaseMs=elapsedMs%periodMs;
  const uint32_t rampMs=phaseMs<halfPeriodMs ? phaseMs : periodMs-phaseMs;
  return minimum+(uint32_t(maximum-minimum)*rampMs)/halfPeriodMs;
}
