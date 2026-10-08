// Former White/Blue: the whole visible string alternates every second.
// Use the same darker orange and purple as Throb, at steady brightness.
uint8_t renderAlternatingColors(uint32_t elapsedMs) {
  const CRGB color = ((elapsedMs / alternatingColorStepMs) % 2 == 0)
      ? CRGB(128, 70, 0) : CRGB(64, 0, 64);
  for (uint16_t i = 0; i < pixelCount; ++i) {
    pixels[i] = color;
  }
  return outputBrightness;
}
