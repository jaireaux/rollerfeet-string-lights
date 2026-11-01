#pragma once
#include <math.h>
#include <stdint.h>
// C++ translation of approved simulator 0.1.25 fall recipes. Pure frame math.
struct FallRgb { double r,g,b; };
constexpr double fallTau=6.283185307179586;
inline double fallWrap(double v){return v-floor(v);}
inline double fallSoft(double d,double w){double a=fmax(0.0,fmin(1.0,1-d/w));return a*a*(3-2*a);}
inline FallRgb fallMix(FallRgb a,FallRgb b,double t){return {a.r+(b.r-a.r)*t,a.g+(b.g-a.g)*t,a.b+(b.b-a.b)*t};}
inline FallRgb fallScale(FallRgb a,double v){return {a.r*v,a.g*v,a.b*v};}
inline FallRgb fallClamp(FallRgb a){return {fmin(255.0,fmax(0.0,a.r)),fmin(255.0,fmax(0.0,a.g)),fmin(255.0,fmax(0.0,a.b))};}
inline uint8_t fallChannel(double v){return uint8_t(lround(fmax(0.0,fmin(255.0,v))));}
// Keep coordinates normalized, matching the simulator (bottom-to-top y).
inline FallRgb fallPixel(uint8_t effect,uint32_t ms,uint16_t i,uint16_t count,double x,double y){
 const double time=double(ms)/1000.0;
 if(effect==0){
  const FallRgb p[]={{190,75,8},{100,120,12},{205,160,18},{170,48,4},{190,115,12}};
  double phase=time/3;return fallScale(p[uint32_t(floor(phase))%5],(1-cos(fallWrap(phase)*fallTau))/2);
 }
 if(effect==1){
  const FallRgb p[]={{100,120,42},{190,115,35},{150,42,35},{205,160,45}};
  double u=double(i)/(count>1?count-1:1),a=(sin(u*fallTau*2-time)+1)/2,b=(sin(u*fallTau*1.4+time*.6)+1)/2;
  return fallScale(fallMix(fallMix(p[0],p[1],a),fallMix(p[2],p[3],b),.45),.20+.80*(.6*a+.4*b));
 }
 if(effect==2){return (uint32_t(floor((i+time*6)/8))%2)?FallRgb{205,160,45}:FallRgb{190,75,22};}
 if(effect==3){
  const FallRgb p[]={{190,75,22},{205,160,45},{150,42,35},{100,120,42},{190,115,35}};
  constexpr double interval=5,speed=15,headPixels=10,fade=3.2;
  int32_t launch=int32_t(floor(time/interval));
  int32_t first=launch-int32_t(ceil(((count+headPixels)/speed+fade)/interval));if(first<0)first=0;
  FallRgb rgb={0,0,0};
  for(int32_t n=first;n<=launch;++n){
   double d=(time-n*interval)*speed-i;
   double glow=d<0?0:d<headPixels?1:fmax(0.0,1-(d-headPixels)/(fade*speed));
   FallRgb c=fallScale(p[n%5],glow);rgb={fmax(rgb.r,c.r),fmax(rgb.g,c.g),fmax(rgb.b,c.b)};
  }return rgb;
 }
 if(effect==4){
  const FallRgb p[]={{190,42,35},{190,75,22},{205,160,45},{110,125,42},{145,68,30},{170,48,30}};
  const double phase=time/(20.0/6),fraction=fallWrap(phase),blend=fraction*fraction*(3-2*fraction);
  uint8_t n=uint32_t(floor(phase))%6;FallRgb color=fallMix(p[n],p[(n+1)%6],blend);
  color=fallScale(color,190/fmax(color.r,fmax(color.g,color.b)));
  const double motion=time*.125,throb=time*.5,angle=fallTau*fmod(motion,1.0);
  double dx=x-.5,dy=y-.5;
  double xx=fallWrap(.5+dx*cos(angle)-dy*sin(angle)+fmod(motion,6.0)/6);
  double yy=fallWrap(.5+dx*sin(angle)+dy*cos(angle)+.5*(1-cos(fallTau*fmod(motion,2.0)/2)));
  uint8_t q=yy<.5?(xx<.5?0:3):(xx<.5?1:2);
  double pulse=fallWrap(throb/3-q/4.0);
  return fallScale(color,.30+.70*(1-fabs(2*pulse-1)));
 }
 // Harvest Cider: same surface for liquid, rising bubble tops and golden pops.
 double simmer=fallTau*fmod(time,15.0)/15,surface=.85+.04*sin(fallTau*x+simmer);
 double level=(35+25*(.5+.5*sin(fallTau*(1.4*x+y)-simmer)))/60*fallSoft(y-surface,.07);
 FallRgb rgb={110*level,55*level,15*level};
 for(uint8_t b=0;b<3;++b){
  double period=6.2+1.1*b,progress=fmod(time+b*1.7,period)/period;
  double cx=.18+.30*b+.07*sin(simmer+b),radius=.025+.07*progress;
  double bubbleSurface=.85+.04*sin(fallTau*cx+simmer),cy=.04+(bubbleSurface-radius-.04)*progress;
  double ring=fallSoft(fabs(hypot(x-cx,y-cy)-radius),.055);
  rgb.r+=100*ring;rgb.g+=120*ring;rgb.b+=42*ring;
  if(progress>.95){double spark=fallSoft(fabs(x-cx),.14)*fallSoft(fabs(y-surface),.10);rgb.r+=205*spark;rgb.g+=160*spark;rgb.b+=45*spark;}
 }
 rgb=fallClamp(rgb);
 if(y>2.0/3){
  double phase=fallTau*fmod(time,9.0)/9,height=(y-2.0/3)*3,center=.5+.13*sin(phase-3*height);
  double wisp=fallSoft(fabs(x-center),.30)+.6*fallSoft(fabs(x-(center+.26)),.195);
  double white=255*(.60+.05*sin(phase))*fmin(wisp,1.0)*fallSoft(1-height,1);
  rgb.r+=white;rgb.g+=white*.90;rgb.b+=white*.72;
 }return fallClamp(rgb);
}
