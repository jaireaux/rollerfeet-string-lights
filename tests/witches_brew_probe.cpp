#include <iostream>
#include "../XIAO_ESP32_holiday_lights/witches_brew_effect.h"
int main() {
  uint32_t ms; float x,y;
  while(std::cin>>ms>>x>>y) {
    const BrewFrame f=brewFrameAt(ms);
    const BrewRgb p=brewRenderForeground(x,y,f,brewRenderBackground(x,y,f));
    std::cout<<unsigned(brewChannel(p.r))<<' '<<unsigned(brewChannel(p.g))<<' '<<unsigned(brewChannel(p.b))<<'\n';
  }
}
