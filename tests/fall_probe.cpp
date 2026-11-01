#include <iostream>
#include "../XIAO_ESP32_holiday_lights/fall_effects.h"
int main(){unsigned effect,ms,i,count;double x,y;while(std::cin>>effect>>ms>>i>>count>>x>>y){auto c=fallPixel(effect,ms,i,count,x,y);std::cout<<unsigned(fallChannel(c.r))<<' '<<unsigned(fallChannel(c.g))<<' '<<unsigned(fallChannel(c.b))<<'\n';}}
