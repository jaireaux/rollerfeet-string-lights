#include <cassert>
#include <cmath>
#include <FastLED.h>
constexpr uint16_t pixelCount=600;
constexpr uint8_t outputBrightness=200;
constexpr uint32_t throbPeriodMs=3000;
constexpr uint8_t throbMinBrightness=10;
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
  unsigned selected=0;
  for(unsigned i=0;i<300;++i) selected+=redSloshBottomLeft(i);
  assert(selected>0 && selected<300);
  assert(renderRedSlosh(0)==10);
  assert(renderRedSlosh(750)==105);
  assert(renderRedSlosh(1500)==200);
  assert(renderRedSlosh(2250)==105);
  assert(renderRedSlosh(3000)==10);
  for(uint32_t t=0;t<22000;t+=31) {
    const uint8_t brightness=renderRedSlosh(t);
    assert(brightness>=10 && brightness<=200);
    for(unsigned i=0;i<600;++i) {
      if(redSloshBottomLeft(i)) {
        assert(pixels[i].r==255 && pixels[i].g==0 && pixels[i].b==4);
      } else if(redSloshTopLeft(i)) {
        assert(pixels[i].r==128 && pixels[i].g==0 && pixels[i].b==128);
      } else if(redSloshTopRight(i)) {
        assert(pixels[i].r==0 && pixels[i].g==128 && pixels[i].b==0);
      } else if(redSloshBottomRight(i)) {
        assert(pixels[i].r==128 && pixels[i].g==70 && pixels[i].b==0);
      } else assert(pixels[i].r==0 && pixels[i].g==0 && pixels[i].b==0);
    }
  }
  // Preserved phase-two mask is still exercised independently.
  for(auto &p:pixels) p=CRGB(200,0,0);
  applyRedSloshShadow(7500);
  for(unsigned i=0;i<300;++i) {
    float distance=std::fabs(spatialMap[i].x/65535.0f-0.5f);
    if(distance<0.07f) assert(pixels[i].r==0);
    if(distance>=0.1f) assert(pixels[i].r==200);
  }
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
