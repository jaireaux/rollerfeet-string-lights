#include <cassert>
#include "../XIAO_ESP32_holiday_lights/status_pixel.h"
int main() {
  StatusIndicator light;
  assert(!light.pixel(0).active);
  light.network(NetworkLight::Searching, 100);
  assert(light.pixel(100).b==100 && light.pixel(350).b==0 && light.pixel(350).active);
  light.network(NetworkLight::Searching, 200); // Polling must not reset blink phase.
  assert(light.pixel(350).b==0);
  light.network(NetworkLight::Connected, 600);
  assert(light.pixel(2599).b==100 && !light.pixel(2600).active);
  light.network(NetworkLight::Offline, 3000);
  assert(light.pixel(6999).r==100 && light.pixel(7000).r==100);
  assert(light.pixel(600000).active && light.pixel(600000).r==100);
  light.update(UpdateLight::Receiving, 8000);
  light.network(NetworkLight::Searching, 8000);
  assert(light.pixel(8000).g==100 && light.pixel(8000).b==0);
  assert(light.pixel(8250).active && light.pixel(8250).g==0);
  light.update(UpdateLight::Complete, 9000);
  assert(light.pixel(10999).g==100);
  assert(light.pixel(11000).b==100); // Network status resumes when OTA expires.
  light.network(NetworkLight::Idle, 12000);
  light.update(UpdateLight::Failed, UINT32_MAX-99);
  assert(light.pixel(UINT32_MAX-99).r==100);
  assert(light.pixel(150).r==0 && light.pixel(150).active);
  assert(!light.pixel(3900).active); // Four-second failure across rollover.
}
