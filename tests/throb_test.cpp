#include <cassert>
#include <cstdint>
struct CRGB {
  uint8_t r = 0, g = 0, b = 0;
  CRGB() = default;
  CRGB(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
};
constexpr uint16_t pixelCount = 300;
constexpr uint32_t throbPeriodMs = 3000;
constexpr uint8_t throbMinBrightness = 10;
constexpr uint8_t outputBrightness = 200;
CRGB pixels[pixelCount];
#include "../XIAO_ESP32_holiday_lights/_throb.ino"
#include "../XIAO_ESP32_holiday_lights/animation_clock.h"
void checkColor(CRGB expected) {
  for (const auto &p : pixels) {
    assert(p.r == expected.r && p.g == expected.g && p.b == expected.b);
  }
}
int main() {
  const CRGB expected[] = {CRGB(128,70,0), CRGB(64,0,64), CRGB(0,50,0)};
  for (uint32_t color = 0; color < 3; ++color) {
    uint8_t previous = 10;
    for (uint32_t phase = 0; phase < 3000; ++phase) {
      const uint8_t brightness = renderThrob(color * 3000 + phase);
      checkColor(expected[color]); // No color change at the brightness peak.
      assert(brightness >= 10 && brightness <= 200);
      if (phase <= 1500) assert(brightness >= previous);
      else assert(brightness <= previous);
      if (phase == 0) assert(brightness == 10);
      if (phase == 750 || phase == 2250) assert(brightness == 105);
      if (phase == 1500) assert(brightness == 200);
      previous = brightness;
    }
  }
  // Controller restart follows green's full pulse, returning to orange at minimum.
  AnimationClock clock;
  uint32_t elapsed;
  clock.start(0);
  assert(clock.frameDue(8950, 50, elapsed));
  renderThrob(elapsed);
  checkColor(expected[2]);
  assert(clock.finished(9000, 3 * throbPeriodMs));
  clock.start(9000);
  assert(clock.frameDue(9000, 50, elapsed));
  assert(elapsed == 0 && renderThrob(elapsed) == 10);
  checkColor(expected[0]);
}
