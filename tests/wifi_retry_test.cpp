#include <cassert>
#include "../XIAO_ESP32_holiday_lights/wifi_retry.h"
int main() {
  WiFiRetry policy(2);
  assert(policy.poll(0, false) == 0);
  assert(policy.poll(9999, false) == -1);
  assert(policy.poll(10000, false) == 1);
  assert(policy.poll(20000, false) == -1);
  assert(policy.poll(39999, false) == -1);
  assert(policy.poll(40000, false) == 0);
  assert(policy.poll(50000, false) == 1);
  assert(policy.poll(51000, true) == -1);
  assert(policy.poll(100000, true) == -1);
  assert(policy.poll(100001, false) == 1); // Rejoin last working network first.
  assert(policy.poll(110001, false) == 0); // Then try the alternate.
  WiFiRetry single(1);
  assert(single.poll(UINT32_MAX-4999, false) == 0);
  assert(single.poll(4999, false) == -1); // Across clock rollover, still attempting.
  assert(single.poll(5000, false) == -1); // Enter cooldown.
  assert(single.poll(24999, false) == -1);
  assert(single.poll(25000, false) == 0);
  WiFiRetry disabled(0);
  assert(disabled.poll(0, false) == -1);
}
