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
  const CRGB colors[]={CRGB(255,0,4),CRGB(255,0,4),CRGB(255,0,4),CRGB(255,0,4)};
  // Each quadrant begins rising on its beat, peaks two beats later, then repeats.
  for(unsigned q=0;q<4;++q) {
    assert(redSloshQuadrantBrightness(q*750,q)==10);
    assert(redSloshQuadrantBrightness(q*750+750,q)==105);
    assert(redSloshQuadrantBrightness(q*750+1500,q)==200);
    assert(redSloshQuadrantBrightness(q*750+3000,q)==10);
  }
  for(uint32_t t=0;t<22000;t+=31) {
    assert(renderRedSlosh(t)==outputBrightness);
    for(unsigned i=0;i<600;++i) {
      if(i>=spatialPixelCount) {
        assert(pixels[i].r==0 && pixels[i].g==0 && pixels[i].b==0);
        continue;
      }
      const unsigned shiftedX=(unsigned(spatialMap[i].x)+(t%6000)*65536u/6000)%65536;
      const bool left=shiftedX<32768, bottom=spatialMap[i].y<32768;
      const unsigned q=bottom?(left?0:3):(left?1:2);
      const uint32_t phase=(t%3000+3000-q*750)%3000;
      const uint32_t ramp=phase<1500?phase:3000-phase;
      const unsigned brightness=10+190*ramp/1500;
      assert(pixels[i].r==colors[q].r*brightness/200);
      assert(pixels[i].g==colors[q].g*brightness/200);
      assert(pixels[i].b==colors[q].b*brightness/200);
    }
  }
  // Half a travel cycle swaps columns; full cycle restores color and pulse phase.
  renderRedSlosh(0);
  CRGB initial[pixelCount];
  for(unsigned i=0;i<pixelCount;++i) initial[i]=pixels[i];
  for(unsigned i=0;i<spatialPixelCount;++i) {
    const unsigned q=redSloshMovingQuadrant(i,0);
    const unsigned opposite[]={3,2,1,0};
    assert(redSloshMovingQuadrant(i,3000)==opposite[q]);
    assert(redSloshMovingQuadrant(i,6000)==q);
  }
  renderRedSlosh(6000);
  for(unsigned i=0;i<pixelCount;++i) {
    assert(pixels[i].r==initial[i].r && pixels[i].g==initial[i].g && pixels[i].b==initial[i].b);
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
