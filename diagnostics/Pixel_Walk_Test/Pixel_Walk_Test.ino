// Standalone diagnostic for XIAO ESP32S3 + 12V WS2811 RGB pixels.
// No Wi-Fi, animations, or skipped section. Serial Monitor: 115200 baud.
#define FASTLED_INTERNAL
#include <FastLED.h>

constexpr uint8_t dataPin = 4;        // XIAO D3 / GPIO4.
constexpr uint16_t scanPixelCount = 1100; // Upper bound, NOT detected length.
constexpr uint32_t pixelHoldMs = 250;
constexpr uint8_t testBrightness = 32;
static_assert(scanPixelCount > 0, "Scan must include at least one pixel");

CRGB pixels[scanPixelCount];
uint16_t currentPixel = 0;
uint32_t lastStepAtMs = 0;

void showCurrentPixel() {
  // Rebuild the entire frame so exactly one addressed pixel is lit.
  fill_solid(pixels, scanPixelCount, CRGB::Black);
  pixels[currentPixel] = CRGB::White;
  Serial.print("Pixel ");
  Serial.print(currentPixel + 1); // Physical count starts at 1.
  Serial.print(" (index ");
  Serial.print(currentPixel);     // Code index starts at 0.
  Serial.println(")");
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, dataPin, RGB>(pixels, scanPixelCount)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(testBrightness);
  Serial.println("Pixel walk: 1 through 1100, one white pixel every 250 ms.");
  Serial.println("Numbers are commanded positions, not automatic detection.");
  lastStepAtMs = millis();
  showCurrentPixel();
}

void loop() {
  const uint32_t now = millis();
  if (now - lastStepAtMs < pixelHoldMs) return;
  lastStepAtMs = now; // No catch-up bursts if a frame is delayed.
  currentPixel = (currentPixel + 1) % scanPixelCount;
  showCurrentPixel();
}
