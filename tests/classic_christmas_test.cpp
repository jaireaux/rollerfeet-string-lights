#include <cassert>
#include <cstdint>
#include <initializer_list>

// Minimal color value stand-in: test the real renderer without LED hardware.
struct CRGB {
  enum Color { Red, Yellow, Blue, Magenta, Orange, Cyan, Green, Black };
  Color value = Black;
  CRGB() = default;
  CRGB(Color color) : value(color) {}
};
constexpr uint16_t pixelCount = 300;
constexpr uint32_t classicChristmasStepMs = 500;
CRGB pixels[pixelCount];
#include "../XIAO_ESP32_holiday_lights/_classicChristmas.ino"

int main() {
  const CRGB::Color expected[] = {CRGB::Red, CRGB::Yellow, CRGB::Blue,
      CRGB::Magenta, CRGB::Orange, CRGB::Cyan, CRGB::Green};
  for (uint32_t time : {0u, 499u, 500u, 3000u, 3500u, 8200u, 14999u}) {
    renderClassicChristmas(time);
    for (uint16_t i = 0; i < pixelCount; ++i) {
      assert(pixels[i].value == expected[(i + time / 500) % 7]);
    }
  }
  // A new run starts consistently, without shared counter carry-over.
  renderClassicChristmas(0);
  assert(pixels[0].value == CRGB::Red);
}
