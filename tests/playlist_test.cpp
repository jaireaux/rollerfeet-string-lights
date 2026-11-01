#include <cassert>
#include <cstring>
#include <cstdint>
#include "../XIAO_ESP32_holiday_lights/status_pixel.h"
StatusPixel fakeStatus={false,0,0,0};
StatusPixel networkStatusPixel(uint32_t) { return fakeStatus; }
void setStatusFrameCallback(void (*)(uint32_t)) {}
bool fakeUpdateBusy=false, fakeRestart=false;
void beginNetworkUpdates() {}
void serviceNetworkUpdates(uint32_t) {}
bool networkUpdateBusy() { return fakeUpdateBusy; }
bool takeAnimationRestartRequest() {
  const bool value=fakeRestart; fakeRestart=false; return value;
}
#include "../XIAO_ESP32_holiday_lights/XIAO_ESP32_holiday_lights.ino"
#include "../XIAO_ESP32_holiday_lights/_throb.ino"
#include "../XIAO_ESP32_holiday_lights/_alternatingColors.ino"
#include "../XIAO_ESP32_holiday_lights/_meteorRain.ino"
#include "../XIAO_ESP32_holiday_lights/_hauntedTide.ino"
#include "../XIAO_ESP32_holiday_lights/_witchfireSparkles.ino"
#include "../XIAO_ESP32_holiday_lights/_redSlosh.ino"
#include "../XIAO_ESP32_holiday_lights/_witchesBrew.ino"
#include "../XIAO_ESP32_holiday_lights/_fall.ino"
int main(){
 setup();
 assert(allAnimationCount==6 && animationCount==6 && automaticCycling && fullPlaylist);
 const char *names[]={"Harvest Glow","Autumn Tide","Orchard Bands","Falling Embers","Cranberry Drift","Harvest Cider"};
 for(unsigned i=0;i<6;i++)assert(!strcmp(allAnimations[i].name,names[i]));
 for(unsigned i=0;i<13;i++){updateAnimation(i*180000u);assert(selectedAnimationId()==i%6);}
 assert(applyLightCommand("animation",4,2500000));assert(selectedAnimationId()==4 && automaticCycling);
 updateAnimation(2679999);assert(selectedAnimationId()==4);
 updateAnimation(2680000);assert(selectedAnimationId()==5);
 assert(applyLightCommand("next",1,2680001) && selectedAnimationId()==0);
 assert(!applyLightCommand("animation",6,2680002));
 assert(applyLightCommand("playlist",0,2700000));assert(selectedAnimationId()==4);
 assert(applyLightCommand("next",1,2700001) && selectedAnimationId()==5);
 assert(applyLightCommand("next",1,2700002) && selectedAnimationId()==4);
 assert(applyLightCommand("playlist",2,2700003) && fullPlaylist && !showPlaylist);
 for(unsigned effect=0;effect<6;++effect){
  allAnimations[effect].render(12345);CRGB sample=pixels[12];
  allAnimations[effect].render(55000);allAnimations[effect].render(12345);
  assert(pixels[12].r==sample.r && pixels[12].g==sample.g && pixels[12].b==sample.b);
  if(effect>=4)for(unsigned i=spatialPixelCount;i<pixelCount;i++)assert(pixels[i].r==0&&pixels[i].g==0&&pixels[i].b==0);
 }
 for(auto &p:pixels)p=CRGB(100,50,25);
 animationBrightness=outputBrightness;brightnessPercent=100;lightsEnabled=true;
 fakeStatus={true,0,0,255};sendCurrentFrame(0);assert(displayedPixels[0].b==255 && pixels[0].r==100);
 fakeStatus={false,0,0,0};assert(applyLightCommand("power",0,2800000));assert(displayedPixels[10].r==0);
 fakeUpdateBusy=true;assert(!applyLightCommand("next",1,2800001));
}
