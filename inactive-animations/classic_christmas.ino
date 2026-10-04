// Render one frame into the buffer. Scheduling and LED output live in
// updateAnimation(); this function neither waits nor changes global timers.
void renderClassicChristmas(uint32_t elapsedMs) {
  static const CRGB palette[] = {
    CRGB::Red, CRGB::Yellow, CRGB::Blue, CRGB::Magenta,
    CRGB::Orange, CRGB::Cyan, CRGB::Green
  };
  constexpr uint8_t paletteSize = sizeof(palette) / sizeof(palette[0]);

  // Time determines position, so dropped frames do not slow the pattern.
  const uint8_t colorOffset = (elapsedMs / classicChristmasStepMs) % paletteSize;
  for (uint16_t i = 0; i < pixelCount; ++i) {
    pixels[i] = palette[(i + colorOffset) % paletteSize];
  }
}
