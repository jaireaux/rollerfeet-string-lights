#include "fall_effects.h"
uint8_t renderFall(uint8_t effect,uint32_t ms){
 for(uint16_t i=0;i<pixelCount;++i){
  if(effect>=4 && i>=spatialPixelCount){pixels[i]=CRGB::Black;continue;}
  double x=i<spatialPixelCount?double(spatialMap[i].x)/65535:0;
  double y=i<spatialPixelCount?double(spatialMap[i].y)/65535:0;
  FallRgb c=fallPixel(effect,ms,i,pixelCount,x,y);
  pixels[i]=CRGB(fallChannel(c.r),fallChannel(c.g),fallChannel(c.b));
 }
 return outputBrightness;
}
uint8_t renderHarvestGlow(uint32_t ms){return renderFall(0,ms);}
uint8_t renderAutumnTide(uint32_t ms){return renderFall(1,ms);}
uint8_t renderOrchardBands(uint32_t ms){return renderFall(2,ms);}
uint8_t renderFallingEmbers(uint32_t ms){return renderFall(3,ms);}
uint8_t renderCranberryDrift(uint32_t ms){
 const uint8_t brightness=renderFall(4,ms);
 // applyRedSloshShadow(ms); // Retained shadow helper in _redSlosh.ino; disabled for fall.
 return brightness;
}
uint8_t renderHarvestCider(uint32_t ms){return renderFall(5,ms);}
