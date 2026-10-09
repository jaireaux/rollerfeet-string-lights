#include <cassert>
#include <cmath>
#include <FastLED.h>
constexpr uint16_t pixelCount=600;
constexpr uint8_t outputBrightness=200;
CRGB pixels[pixelCount];
#include "../XIAO_ESP32_holiday_lights/_redSlosh.ino"
int main() {
  assert(std::fabs(redShadowCenter(7500)-0.5f)<0.0001f);
  assert(std::fabs(redShadowCenter(11500)-0.5f)<0.0001f);
  assert(redShadowCenter(15500)==0.5f && redShadowCenter(16499)==0.5f);
  assert(redShadowCenter(20500)==0.5f && redShadowCenter(21499)==0.5f);
  assert(std::fabs(redShadowCenter(17000)+0.1f)<0.0001f);
  assert(redShadowCenter(22000)==redShadowCenter(0));
  for(uint32_t t=0;t<22000;t+=31) {
    renderRedSlosh(t);
    for(unsigned i=0;i<300;++i) {
      assert(pixels[i].g==0 && pixels[i].b<=4);
      if(t<7000) assert(pixels[i].r>=191);
    }
    for(unsigned i=300;i<600;++i) assert(pixels[i].r==0 && pixels[i].g==0 && pixels[i].b==0);
  }
  renderRedSlosh(7500);
  bool shadow=false,bright=false;
  for(unsigned i=0;i<300;++i) {float x=spatialMap[i].x/65535.0f;
    if(std::fabs(x-0.5f)<0.07f) {assert(pixels[i].r==0);shadow=true;}
    if(std::fabs(x-0.5f)>0.1f) {assert(pixels[i].r>=191);bright=true;}
  }
  assert(shadow && bright && FastLED.frames==0);
  renderRedSlosh(UINT32_MAX);
}
