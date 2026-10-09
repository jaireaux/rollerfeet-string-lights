#include <math.h>
#include "spatial_map.h"

float brewSoft(float distance, float width) {
  float a=1-distance/width;
  if(a<0) return 0;
  if(a>1) return 1;
  return a*a*(3-2*a);
}
uint8_t renderWitchesBrew(uint32_t elapsedMs) {
  constexpr float tau=6.28318530718f;
  const float simmer=tau*float(elapsedMs%15000)/15000.0f;
  const float steamPhase=tau*float(elapsedMs%9000)/9000.0f;
  for(uint16_t i=0;i<pixelCount;++i) {
    if(i>=spatialPixelCount) {pixels[i]=CRGB::Black;continue;}
    const float x=spatialMap[i].x/65535.0f,y=spatialMap[i].y/65535.0f;
    const float surface=0.40f+0.04f*sinf(tau*x+simmer);
    const float pool=brewSoft(y-surface,0.07f);
    const float green=(35+25*(0.5f+0.5f*sinf(tau*(1.4f*x+y)-simmer)))*pool;
    float red=0,blue=0,g=green;
    // Three staggered rising/expanding bubbles, with independent periods.
    for(uint8_t b=0;b<3;++b) {
      const uint32_t period=6200+1100*b;
      const uint32_t clock=elapsedMs+uint32_t(b)*1700;
      const float progress=float(clock%period)/period;
      const float cx=0.18f+0.30f*b+0.07f*sinf(simmer+b);
      const float cy=0.04f+0.40f*progress;
      const float radius=0.025f+0.07f*progress;
      const float dx=x-cx,dy=y-cy;
      const float bubble=brewSoft(fabsf(sqrtf(dx*dx+dy*dy)-radius),0.055f);
      red+=105*bubble;blue+=140*bubble;
      // A short orange burst near the surface as each bubble pops.
      if(progress>0.95f) {
        const float spark=brewSoft(fabsf(x-cx),0.14f)*brewSoft(fabsf(y-(surface+0.08f)),0.10f);
        red+=200*spark;g+=65*spark;
      }
    }
    // Soft-white drifting wisps only in the upper third; cores breathe 55–65%.
    if(y>2.0f/3) {
      const float height=(y-2.0f/3)*3;
      const float center=0.5f+0.13f*sinf(steamPhase-3*height);
      const float wisp=brewSoft(fabsf(x-center),0.20f)
                      +0.6f*brewSoft(fabsf(x-(center+0.26f)),0.13f);
      const float fade=brewSoft(1-height,1.0f);
      const float white=255*(0.60f+0.05f*sinf(steamPhase))*fminf(wisp,1.0f)*fade;
      red+=white;g+=white;blue+=white;
    }
    pixels[i]=CRGB(uint8_t(fminf(red,255)),uint8_t(fminf(g,255)),uint8_t(fminf(blue,255)));
  }
  return outputBrightness;
}
