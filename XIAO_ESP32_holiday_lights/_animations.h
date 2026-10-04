#ifndef ANIMATIONS_H_INCLUDED
#define ANIMATIONS_H_INCLUDED

  // int add(int a, int b);  // Function prototype, its declaration
  void redgreen();
  void solidglow();
  void throb();
  void whiteblue();
  void pacifica();    
  void metaballs();
  byte dist (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
  uint16_t XY (uint8_t x, uint8_t y);
  void classicChristmas();
  void peppermint();
  void setPixel(int Pixel, byte red, byte green, byte blue);
  void setAll(byte red, byte green, byte blue);
  void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
  void fadeToBlack(int ledNo, byte fadeValue);
  void fadeToBlackMeta(int ledNo, byte fadeValue);
  void setPixelMeta(int i, byte red, byte green, byte blue);
  void RGBLoop();
#endif
