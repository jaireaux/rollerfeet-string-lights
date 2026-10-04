#include <cassert>
#include <cstdint>
struct CRGB {
  enum Color { DarkOrange, Purple, Black };
  Color value = Black;
  CRGB() = default;
  CRGB(Color c) : value(c) {}
};
constexpr uint16_t pixelCount = 300;
constexpr uint32_t throbPeriodMs = 3000;
constexpr uint8_t throbMinBrightness = 10;
constexpr uint8_t outputBrightness = 200;
CRGB pixels[pixelCount];
#include "../XIAO_ESP32_holiday_lights/_throb.ino"
int main() {
  assert(renderThrob(0) == 10);
  assert(pixels[0].value == CRGB::DarkOrange);
  assert(renderThrob(750) == 105);
  assert(renderThrob(1500) == 200);
  assert(pixels[0].value == CRGB::Purple);
  assert(renderThrob(2250) == 105);
  assert(renderThrob(3000) == 10);
  assert(pixels[0].value == CRGB::DarkOrange);
  uint8_t previous = 10;
  for (uint32_t t = 0; t <= 3000; ++t) {
    const uint8_t brightness = renderThrob(t);
    assert(brightness >= 10 && brightness <= 200);
    if (t <= 1500) assert(brightness >= previous);
    else assert(brightness <= previous);
    previous = brightness;
    for (const auto &pixel : pixels) {
      assert(pixel.value == ((t % 3000 < 1500) ? CRGB::DarkOrange : CRGB::Purple));
    }
    assert(renderThrob(t + 12000) == brightness);
  }
}
