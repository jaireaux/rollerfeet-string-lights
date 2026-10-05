#pragma once
#include <stdint.h>

// Polling policy only: never waits or performs network I/O.
class WiFiRetry {
 public:
  explicit WiFiRetry(uint8_t count) : count_(count) {}
  int poll(uint32_t now, bool connected) {
    if (connected) {
      wasConnected_ = true;
      attempting_ = cooling_ = false;
      attempts_ = 0;
      return -1;
    }
    if (!count_) return -1;
    if (wasConnected_) {
      wasConnected_ = false;
      attempting_ = cooling_ = false;
    }
    if (attempting_) {
      if (uint32_t(now - started_) < attemptMs) return -1;
      attempting_ = false;
      current_ = (current_ + 1) % count_;
      if (++attempts_ >= count_) {
        cooling_ = true;
        started_ = now;
        attempts_ = 0;
        return -1;
      }
    }
    if (cooling_) {
      if (uint32_t(now - started_) < retryPauseMs) return -1;
      cooling_ = false;
    }
    attempting_ = true;
    started_ = now;
    return current_;
  }
  static constexpr uint32_t attemptMs = 10000;
  static constexpr uint32_t retryPauseMs = 20000;
 private:
  uint8_t count_, current_ = 0, attempts_ = 0;
  bool attempting_ = false, cooling_ = false, wasConnected_ = false;
  uint32_t started_ = 0;
};
