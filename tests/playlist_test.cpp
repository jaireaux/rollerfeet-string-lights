#include <cassert>
#include "../XIAO_ESP32_holiday_lights/XIAO_ESP32_holiday_lights.ino"
#include "../XIAO_ESP32_holiday_lights/_throb.ino"
#include "../XIAO_ESP32_holiday_lights/_alternatingColors.ino"
#include "../XIAO_ESP32_holiday_lights/_meteorRain.ino"
#include "../XIAO_ESP32_holiday_lights/_hauntedTide.ino"
bool dark(const CRGB &p) {return p.r==0 && p.g==0 && p.b==0;}
int main() {
  static_assert(pixelCount == (HOLIDAY_LIGHTS_PRODUCTION ? 300 : 100), "Profile length");
  setup();
  updateAnimation(0);
  assert(currentAnimationIndex==0 && FastLED.frames==1);
  updateAnimation(1);
  assert(FastLED.frames==1);
  updateAnimation(animationDurationMs);
  assert(currentAnimationIndex==1 && FastLED.brightness==200);
  for (int i=0;i<pixelCount;++i)
    assert(dark(pixels[i]) == (i>=skippedPixelBegin && i<skippedPixelEnd));
  updateAnimation(2*animationDurationMs);
  assert(currentAnimationIndex==2 && pixels[0].r==128);
  for (int i=1;i<pixelCount;++i) assert(dark(pixels[i])); // No preceding frame remnants.
#if HOLIDAY_LIGHTS_PRODUCTION
  // Meteor crosses the hidden section on physical indices.
  updateAnimation(2*animationDurationMs+3600);
  for (int i=210;i<252;++i) assert(dark(pixels[i]));
  updateAnimation(2*animationDurationMs+4300);
  assert(pixels[252].r>0);
#endif
  updateAnimation(3*animationDurationMs);
  assert(currentAnimationIndex==3 && FastLED.brightness==outputBrightness);
  for (int i=0;i<pixelCount;++i)
    if (i>=skippedPixelBegin && i<skippedPixelEnd) assert(dark(pixels[i]));
  updateAnimation(4*animationDurationMs);
  assert(currentAnimationIndex==0 && FastLED.brightness==10);
  // Fixed position and fade at a known time, regardless of prior rendering.
  renderMeteorRain(1000); // Head at pixel 60, ten-pixel body, 800 ms trail.
  assert(pixels[60].r==128 && pixels[51].r==128 && dark(pixels[61]));
  assert(pixels[30].r>0 && pixels[30].r<pixels[50].r && dark(pixels[0]));
  const CRGB sample=pixels[30];
  renderMeteorRain(4000);
  renderMeteorRain(1000);
  assert(pixels[30].r==sample.r && pixels[30].g==sample.g);
  constexpr uint32_t launchInterval = HOLIDAY_LIGHTS_PRODUCTION ? 3325 : 1659;
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
}
