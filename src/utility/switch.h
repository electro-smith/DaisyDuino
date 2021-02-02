#ifndef DSY_SWITCH_H
#define DSY_SWITCH_H

#include "Arduino.h"

namespace daisy {
class Switch {
public:
  Switch() {}
  ~Switch() {}

  void Init(float update_rate, bool invert, uint8_t pin, uint8_t mode);

  // debounces and processes input
  void Debounce();

  bool RisingEdge() { return state_ == 0x7f; }

  bool FallingEdge() { return state_ == 0x80; }

  bool Pressed() { return state_ == 0xff; }

  float TimeHeldMs() { return Pressed() ? time_held_ * 1000.f : 0; }

private:
  bool flip_;
  float time_per_update_, time_held_;
  uint8_t state_, pin_;
};
} // namespace daisy
#endif