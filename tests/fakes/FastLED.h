#pragma once
#include <cstdint>
struct CRGB {
  uint8_t r=0,g=0,b=0;
  enum Color { Black };
  CRGB() = default;
  CRGB(Color) {}
  CRGB(uint8_t red,uint8_t green,uint8_t blue):r(red),g(green),b(blue){}
};
struct WS2811 {};
constexpr int RGB=0, TypicalLEDStrip=0;
struct FakeLED {
  unsigned frames=0;
  uint8_t brightness=0;
  template<class Chip, uint8_t Pin, int Order>
  FakeLED &addLeds(CRGB *, uint16_t) { return *this; }
  void setCorrection(int) {}
  void setBrightness(uint8_t value) { brightness=value; }
  void show() { ++frames; }
} FastLED;
struct FakeSerial { void begin(int) {} void println(const char *) {} } Serial;
uint32_t testNow=0;
uint32_t millis() { return testNow; }
