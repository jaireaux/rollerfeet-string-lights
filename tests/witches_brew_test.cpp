#include <cassert>
#include <cmath>
#include <initializer_list>
#include <FastLED.h>
constexpr uint16_t pixelCount=600;
constexpr uint8_t outputBrightness=200;
CRGB pixels[pixelCount];
#include "../XIAO_ESP32_holiday_lights/_witchesBrew.ino"
int main() {
  assert(std::fabs(brewSurfaceAt(0,0)-.80f)<1e-6f);
  assert(std::fabs(brewSurfaceAt(.25f,0)-.84f)<1e-6f);
  for(float surface : {.76f,.80f,.84f}) {
    assert(std::fabs(brewBubbleCenterY(0,surface,.025f)-.04f)<1e-6f);
    assert(std::fabs(brewBubbleCenterY(1,surface,.095f)+.095f-surface)<1e-6f);
  }
  for(uint32_t t : {0u,1500u,5891u,6200u,30000u,123456u,UINT32_MAX}) {
    const BrewFrame f=brewFrameAt(t);
    for(const auto &bubble:f.bubbles) {
      assert(bubble.cy>=.04f && bubble.cy+bubble.radius<=brewSurfaceAt(bubble.cx,f.simmer)+1e-5f);
    }
    // Foreground is absent below its region and adds equal white to the base
    // before channel saturation. Wider wisp reaches x=.25 at this phase/height.
    const BrewRgb base={2,3,4};
    auto low=brewRenderForeground(.25f,.5f,f,base);
    assert(low.r==2 && low.g==3 && low.b==4);
    for(auto &p:pixels)p=CRGB(255,255,255);
    assert(renderWitchesBrew(t)==outputBrightness);
    for(unsigned i=spatialPixelCount;i<pixelCount;++i)assert(!pixels[i].r&&!pixels[i].g&&!pixels[i].b);
  }
  const BrewFrame f=brewFrameAt(0);
  auto wide=brewRenderForeground(.25f,.8f,f,{0,0,0});
  assert(wide.r>0 && wide.r==wide.g && wide.g==wide.b);
  renderWitchesBrew(1000);const CRGB first=pixels[0];
  renderWitchesBrew(20000);renderWitchesBrew(1000);
  assert(pixels[0].r==first.r && pixels[0].g==first.g && pixels[0].b==first.b);
  assert(FastLED.frames==0);
}
