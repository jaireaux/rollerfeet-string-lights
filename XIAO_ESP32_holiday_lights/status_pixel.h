#pragma once
#include <stdint.h>

struct StatusPixel { bool active; uint8_t r, g, b; };
enum class NetworkLight { Idle, Searching, Connected, Offline };
enum class UpdateLight { Idle, Receiving, Complete, Failed };

class StatusIndicator {
 public:
  void network(NetworkLight state, uint32_t now) {
    if (state != network_) { network_ = state; networkStarted_ = now; }
  }
  void update(UpdateLight state, uint32_t now) {
    update_ = state;
    updateStarted_ = now;
  }
  StatusPixel pixel(uint32_t now) const {
    const uint32_t updateAge = now - updateStarted_;
    if (update_ == UpdateLight::Receiving) return blink(updateAge, 0, 100, 0);
    if (update_ == UpdateLight::Complete && updateAge < confirmationMs)
      return {true, 0, 100, 0};
    if (update_ == UpdateLight::Failed && updateAge < failureMs)
      return blink(updateAge, 100, 0, 0);
    const uint32_t networkAge = now - networkStarted_;
    if (network_ == NetworkLight::Searching) return blink(networkAge, 0, 0, 100);
    if (network_ == NetworkLight::Connected && networkAge < confirmationMs)
      return {true, 0, 0, 100};
    if (network_ == NetworkLight::Offline && networkAge < failureMs)
      return {true, 100, 0, 0};
    return {false, 0, 0, 0};
  }
  static constexpr uint32_t confirmationMs = 2000;
  static constexpr uint32_t failureMs = 4000;
  static constexpr uint32_t blinkHalfMs = 250;
 private:
  static StatusPixel blink(uint32_t age, uint8_t r, uint8_t g, uint8_t b) {
    const bool on = (age / blinkHalfMs) % 2 == 0;
    return {true, uint8_t(on ? r : 0), uint8_t(on ? g : 0), uint8_t(on ? b : 0)};
  }
  NetworkLight network_ = NetworkLight::Idle;
  UpdateLight update_ = UpdateLight::Idle;
  uint32_t networkStarted_ = 0, updateStarted_ = 0;
};
