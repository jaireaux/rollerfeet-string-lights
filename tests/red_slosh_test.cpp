#include <cassert>
#include <cmath>
#include <FastLED.h>
constexpr uint16_t pixelCount=600;
constexpr uint8_t outputBrightness=200;
CRGB pixels[pixelCount];
#include "../XIAO_ESP32_holiday_lights/_redSlosh.ino"
#include "../XIAO_ESP32_holiday_lights/_witchesBrew.ino"
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
      assert(pixels[i].r>=191); // No shadow at any phase in troubleshooting build.
    }
    for(unsigned i=300;i<600;++i) assert(pixels[i].r==0 && pixels[i].g==0 && pixels[i].b==0);
  }
  renderRedSlosh(7500);
  applyRedSloshShadow(7500); // Exercise preserved mask separately.
  bool shadow=false,bright=false;
  for(unsigned i=0;i<300;++i) {float x=spatialMap[i].x/65535.0f;
    if(std::fabs(x-0.5f)<0.07f) {assert(pixels[i].r==0);shadow=true;}
    if(std::fabs(x-0.5f)>0.1f) {assert(pixels[i].r>=191);bright=true;}
  }
  assert(shadow && bright && FastLED.frames==0);
  // Continuous waves have bounded temporal and spatial gradients.
  for(uint32_t t=0;t<40000;t+=113) {
    for(float x=0;x<=1;x+=0.1f) {
      float v=redSloshLevel(t,x,0.4f);
      assert(v>=0.75f && v<=1.0f);
      assert(std::fabs(v-redSloshLevel(t+30,x,0.4f))<0.002f);
      assert(std::fabs(v-redSloshLevel(t,x+0.01f,0.4f))<0.004f);
    }
  }
  // The stationary midpoint shadow does not freeze uncovered red pixels.
  CRGB before[300];
  renderRedSlosh(15500);
  applyRedSloshShadow(15500);
  for(unsigned i=0;i<300;++i) before[i]=pixels[i];
  renderRedSlosh(16499);
  applyRedSloshShadow(16499);
  unsigned changedOutside=0;
  for(unsigned i=0;i<300;++i) {
    float x=spatialMap[i].x/65535.0f;
    if(std::fabs(x-0.5f)>=0.1f) {
      changedOutside+=before[i].r!=pixels[i].r;
      assert(pixels[i].r==uint8_t(255*redSloshLevel(16499,x,spatialMap[i].y/65535.0f)));
    }
  }
  assert(changedOutside>0);
  renderRedSlosh(UINT32_MAX);
  bool green=false,purple=false;
  for(uint32_t t=0;t<30000;t+=137) {
    renderWitchesBrew(t);
    for(unsigned i=0;i<300;++i) {
      if(spatialMap[i].y/65535.0f>2.0f/3) {
        assert(pixels[i].r==pixels[i].g && pixels[i].g==pixels[i].b);
        assert(pixels[i].r<=166); // White steam <=65% before output cap.
      }
      green |= pixels[i].g>pixels[i].r;
      purple |= pixels[i].b>pixels[i].g;
    }
    for(unsigned i=300;i<600;++i) assert(pixels[i].r==0 && pixels[i].g==0 && pixels[i].b==0);
  }
  assert(green && purple && FastLED.frames==0);
  assert(brewSoft(0,1)==1 && brewSoft(2,1)==0);
  renderWitchesBrew(UINT32_MAX);
}
