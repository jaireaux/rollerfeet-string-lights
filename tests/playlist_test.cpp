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
bool dark(const CRGB &p) {return p.r==0 && p.g==0 && p.b==0;}
int main() {
  static_assert(pixelCount == 600, "Profile length");
  setup();
  assert(fullPlaylist && showPlaylist);
  assert(applyLightCommand("playlist", HOLIDAY_LIGHTS_PRODUCTION ? 1 : 0, 0));
  FastLED.frames=0;animationClock.start(0);
  updateAnimation(0);
  assert(currentAnimationIndex==0 && FastLED.frames==1);
  updateAnimation(1);
  assert(FastLED.frames==1);
#if HOLIDAY_LIGHTS_PRODUCTION
  assert(animationCount==7 && !strcmp(animations[3].name,"Meteor Rain"));
#else
  assert(animationCount==2 && !strcmp(animations[0].name,"Red Slosh") &&
      !strcmp(animations[1].name,"Witch's Brew"));
#endif
  for (uint8_t effect=1;effect<=animationCount;++effect) {
    updateAnimation(uint32_t(effect)*animationDurationMs);
    assert(currentAnimationIndex==effect%animationCount);

  }
  for (auto &p:pixels) p=CRGB(100,50,25);
  sendCurrentFrame(0);
  assert(pixels[skippedPixelBegin].r==100 && displayedPixels[skippedPixelBegin].r>0);
  // Fixed position and fade at a known time, regardless of prior rendering.
  renderMeteorRain(1000); // Head at pixel 60, ten-pixel body, 800 ms trail.
  assert(pixels[60].r==128 && pixels[51].r==128 && dark(pixels[61]));
  assert(pixels[30].r>0 && pixels[30].r<pixels[50].r && dark(pixels[0]));
  const CRGB sample=pixels[30];
  renderMeteorRain(4000);
  renderMeteorRain(1000);
  assert(pixels[30].r==sample.r && pixels[30].g==sample.g);
  constexpr uint32_t launchInterval = 1250;
  renderMeteorRain(launchInterval-1);
  assert(dark(pixels[0])); // No next launch yet.
  renderMeteorRain(launchInterval); // New purple head while orange remains ahead.
  assert(pixels[0].r==64 && pixels[0].b==64);
  const uint16_t olderHead = launchInterval * 60 / 1000;
  assert(pixels[olderHead].r==128 && pixels[olderHead].g==70);
  renderMeteorRain(2*launchInterval); // Third launch is green.
  assert(pixels[0].g==50 && pixels[0].r==0);
  renderMeteorRain(3*launchInterval); // Wrap to orange.
  assert(pixels[0].r==128 && pixels[0].g==70);
  renderMeteorRain(0); // Restart drops all previous launches.
  assert(pixels[0].r==128);
  for (int i=1;i<pixelCount;++i) assert(dark(pixels[i]));
  assert(!strcmp(allAnimations[1].name,"Haunted Tide"));
  // Haunted Tide never sends frames, respects color bounds, and ignores old pixels.
  const unsigned framesBefore = FastLED.frames;
  CRGB reference[pixelCount];
  renderHauntedTide(12345);
  for (int i=0;i<pixelCount;++i) reference[i]=pixels[i];
  for (auto &p:pixels) p=CRGB(255,255,255);
  renderHauntedTide(12345);
  for (int i=0;i<pixelCount;++i) {
    assert(pixels[i].r==reference[i].r && pixels[i].g==reference[i].g && pixels[i].b==reference[i].b);
  }
  bool changed=false;
  for (uint32_t t=0;t<180000;t+=137) {
    renderHauntedTide(t);
    for (int i=0;i<pixelCount;++i) {
      assert(pixels[i].r<=128 && pixels[i].g<=70 && pixels[i].b<=64);
      changed |= pixels[i].r!=reference[i].r || pixels[i].g!=reference[i].g || pixels[i].b!=reference[i].b;
    }
  }
  assert(changed && FastLED.frames==framesBefore);
  renderHauntedTide(UINT32_MAX); // No overflow in staggered firefly clocks.
  // Flashes contain 1–3 glints per region; the following frame is background only.
  bool previouslyLit[(pixelCount+24)/25] = {};
  unsigned flashCount=0;
  for (uint32_t t=0;t<18000;t+=sparkleFrameMs) {
    const CRGB base=sparkleBackground(t);
    renderWitchfireSparkles(t);
    for (int start=0;start<pixelCount;start+=25) {
      int glints=0;
      for (int i=start;i<start+25 && i<pixelCount;++i) {
        glints += pixels[i].r!=base.r || pixels[i].g!=base.g || pixels[i].b!=base.b;
      }
      assert(glints<=3);
      if (previouslyLit[start/25]) assert(glints==0);
      previouslyLit[start/25] = glints>0;
      flashCount += glints>0;
    }
  }
  assert(flashCount>0);
  renderWitchfireSparkles(1234);
  for (int i=0;i<pixelCount;++i) reference[i]=pixels[i];
  renderWitchfireSparkles(UINT32_MAX);
  renderWitchfireSparkles(1234);
  for (int i=0;i<pixelCount;++i)
    assert(pixels[i].r==reference[i].r && pixels[i].g==reference[i].g && pixels[i].b==reference[i].b);
  assert(FastLED.frames==framesBefore);
  // OTA pauses output; a handled failure requests a clean animation restart.
  fakeUpdateBusy=true;
  testNow=200000;
  loop();
  assert(FastLED.frames==framesBefore);
  const uint8_t previousEffect=currentAnimationIndex;
  fakeUpdateBusy=false;
  fakeRestart=true;
  loop();
  assert(FastLED.frames==framesBefore+1 && currentAnimationIndex==previousEffect);
  assert(!fakeRestart);
  // Status owns only physical pixel #1 and never changes the animation buffer.
  animationBrightness=10;
  for (auto &p:pixels) p=CRGB(100,50,25);
  fakeStatus={true,0,0,100};
  sendCurrentFrame(testNow);
  assert(displayedPixels[0].b==100 && displayedPixels[0].r==0);
  assert(displayedPixels[1].r==5 && displayedPixels[1].g==2 && displayedPixels[1].b==1);
  assert(pixels[0].r==100 && pixels[1].r==100);
  fakeStatus={false,0,0,0};
  refreshStatusFrame(testNow+1);
  assert(displayedPixels[0].r==5 && displayedPixels[0].b==1);
  // Web commands expose all effects, restart the selected animation and keep cycling, and reject invalid changes.
  assert(!applyLightCommand("animation", 7, 200001));
  assert(!applyLightCommand("brightness", 101, 200001));
  assert(!applyLightCommand("duration", 9, 200001));
  assert(!applyLightCommand("duration", 601, 200001));
  assert(!applyLightCommand("power", 2, 200001));
  assert(!applyLightCommand("unknown", 0, 200001));
  assert(applyLightCommand("animation", 3, 200001));
  assert(automaticCycling && fullPlaylist && selectedAnimationId()==3);
  assert(animationClock.animationStartedAtMs==200001);
  updateAnimation(200001 + runtimeDurationMs - 1);assert(selectedAnimationId()==3);
  updateAnimation(200001 + runtimeDurationMs);assert(selectedAnimationId()==4);
  assert(applyLightCommand("animation", 3, 599000));
  assert(applyLightCommand("next", 1, 600000) && selectedAnimationId()==4);
  assert(applyLightCommand("playlist", 1, 600001));
  assert(automaticCycling && fullPlaylist && selectedAnimationId()==0);
  assert(applyLightCommand("duration", 120, 600002));
  updateAnimation(720001); assert(selectedAnimationId()==0);
  updateAnimation(720002); assert(selectedAnimationId()==1);
  assert(applyLightCommand("auto", 0, 720003));
  assert(!automaticCycling && selectedAnimationId()==1);
  assert(applyLightCommand("playlist", 0, 720004));
  assert(automaticCycling && !fullPlaylist && selectedAnimationId()==5);
  assert(applyLightCommand("next", 1, 720004) && selectedAnimationId()==6);
  assert(applyLightCommand("next", 1, 720004) && selectedAnimationId()==5);
  fakeUpdateBusy=true;
  assert(!applyLightCommand("power", 0, 720005) && lightsEnabled);
  fakeUpdateBusy=false;
  assert(applyLightCommand("power", 0, 720006));
  for (const auto &p:displayedPixels) assert(dark(p));
  fakeStatus={true,100,0,0};sendCurrentFrame(720007);
  assert(displayedPixels[0].r==100 && dark(displayedPixels[1]));
  fakeStatus={false,0,0,0};
  assert(applyLightCommand("power", 1, 720008));
  assert(applyLightCommand("brightness", 25, 720009));
  animationBrightness=outputBrightness;
  for (auto &p:pixels) p=CRGB(100,80,40);
  sendCurrentFrame(720009);
  assert(displayedPixels[1].r==25 && displayedPixels[1].g==20 && displayedPixels[1].b==10);
  char json[384];writeLightState(json,sizeof(json));
  assert(strstr(json,"\"brightness\":25") && strstr(json,"\"duration\":120"));
  // Six-effect show excludes Red Slosh and wraps from Brew to Throb.
  assert(applyLightCommand("duration",10,800000));
  assert(applyLightCommand("playlist",2,800000));
  const unsigned expected[]={0,1,2,3,4,6,0};
  for(unsigned i=0;i<7;++i) {
    updateAnimation(800000+i*10000);
    assert(selectedAnimationId()==expected[i]);
  }
  assert(applyLightCommand("animation",5,900000));
  assert(!showPlaylist && selectedAnimationId()==5);
  // Quadrant Throb must reach the actual output buffer via controller brightness.
  assert(applyLightCommand("brightness",100,1000000));
  unsigned quadrantPixel=0;
  while(quadrantPixel<pixelCount && (!redSloshBottomLeft(quadrantPixel) || spatialMap[quadrantPixel].x>=16384)) ++quadrantPixel;
  assert(quadrantPixel<pixelCount);
  animationBrightness=renderRedSlosh(0);sendCurrentFrame(1000000);
  assert(displayedPixels[quadrantPixel].r==76);
  animationBrightness=renderRedSlosh(1500);sendCurrentFrame(1001500);
  assert(displayedPixels[quadrantPixel].r==165);
  char tiny[4];writeLightState(tiny,sizeof(tiny));assert(tiny[3]=='\0');

}
