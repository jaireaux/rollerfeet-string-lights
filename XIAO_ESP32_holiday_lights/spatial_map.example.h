#pragma once
#include <stdint.h>
// Generic illustration only. Copy to spatial_map.h, then supply your layout.
// Physical #1 = index 0; normalized 16-bit x left-to-right/y bottom-to-top.
struct SpatialPoint { uint16_t x, y; };
constexpr SpatialPoint spatialMap[] = {{0,0}, {32768,32768}, {65535,65535}};
constexpr uint16_t spatialPixelCount = sizeof(spatialMap)/sizeof(spatialMap[0]);
