#include <cassert>
#include "../XIAO_ESP32_holiday_lights/animation_clock.h"

int main() {
  AnimationClock clock;
  uint32_t elapsed;
  clock.start(1000);
  assert(clock.frameDue(1000, 15000, 500, elapsed) && elapsed == 0);
  assert(!clock.frameDue(1499, 15000, 500, elapsed));
  assert(clock.frameDue(1500, 15000, 500, elapsed) && elapsed == 500);
  // A delayed loop produces one current frame, not a catch-up burst.
  assert(clock.frameDue(4200, 15000, 500, elapsed) && elapsed == 3200);
  assert(!clock.frameDue(4200, 15000, 500, elapsed));
  // Duration expires even if the next regular frame was not due.
  clock.start(1000);
  assert(clock.frameDue(15999, 15000, 500, elapsed));
  assert(clock.frameDue(16000, 15000, 500, elapsed) && elapsed == 0);
  assert(!clock.frameDue(16001, 15000, 500, elapsed));
  // Both frame and lifetime arithmetic work across the 32-bit rollover.
  clock.start(UINT32_MAX - 249);
  assert(clock.frameDue(UINT32_MAX - 249, 15000, 500, elapsed));
  assert(!clock.frameDue(249, 15000, 500, elapsed));
  assert(clock.frameDue(250, 15000, 500, elapsed) && elapsed == 500);
  assert(clock.frameDue(14750, 15000, 500, elapsed) && elapsed == 0);
}
