#include <cassert>
#include "../XIAO_ESP32_holiday_lights/animation_clock.h"
int main() {
  AnimationClock clock;
  uint32_t elapsed;
  clock.start(1000);
  assert(clock.frameDue(1000, 500, elapsed) && elapsed == 0);
  assert(!clock.frameDue(1499, 500, elapsed));
  assert(clock.frameDue(1500, 500, elapsed) && elapsed == 500);
  assert(clock.frameDue(4200, 500, elapsed) && elapsed == 3200);
  assert(!clock.frameDue(4200, 500, elapsed));
  assert(!clock.finished(9999, 9000));
  assert(clock.finished(10000, 9000));
  // Switching effects changes the interval and makes the first frame immediate.
  clock.start(10000);
  assert(clock.frameDue(10000, 1000, elapsed) && elapsed == 0);
  assert(!clock.frameDue(10999, 1000, elapsed));
  assert(clock.frameDue(11000, 1000, elapsed) && elapsed == 1000);
  assert(!clock.finished(25999, 16000));
  assert(clock.finished(26000, 16000));
  clock.start(UINT32_MAX - 249);
  assert(clock.frameDue(UINT32_MAX - 249, 500, elapsed));
  assert(!clock.frameDue(249, 500, elapsed));
  assert(clock.frameDue(250, 500, elapsed) && elapsed == 500);
  assert(!clock.finished(8749, 9000));
  assert(clock.finished(8750, 9000));
}
