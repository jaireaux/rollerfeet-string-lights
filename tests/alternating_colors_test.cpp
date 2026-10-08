#include <cassert>
#include <cstdint>
#include <initializer_list>
struct CRGB {
  uint8_t r=0,g=0,b=0;
  CRGB() = default;
  CRGB(uint8_t red,uint8_t green,uint8_t blue):r(red),g(green),b(blue){}
};
constexpr uint16_t pixelCount=300;
constexpr uint32_t alternatingColorStepMs=1000;
constexpr uint8_t outputBrightness=200;
CRGB pixels[pixelCount];
#include "../XIAO_ESP32_holiday_lights/_alternatingColors.ino"
int main() {
  for (uint32_t t : {0u,999u,1000u,1999u,2000u,14999u,15000u,15999u}) {
    assert(renderAlternatingColors(t)==200);
    bool orange=(t/1000)%2==0;
    for (const auto &p:pixels) {
      assert(p.r==(orange?128:64));
      assert(p.g==(orange?70:0));
      assert(p.b==(orange?0:64));
    }
  }
}
