#ifndef DSY_ENCODER_H
#define DSY_ENCODER_H

#include "Arduino.h"
#include "switch.h"

namespace daisy {

class Encoder {
public:
  Encoder() {}
  ~Encoder() {}

  void Init(float update_rate, uint8_t pinA, uint8_t pinB, uint8_t pinClick,
            uint8_t modeA, uint8_t modeB, uint8_t modeC);

  void Debounce();

  int32_t Increment() { return inc_; }

  bool RisingEdge() { return encSwitch.RisingEdge(); }

  bool FallingEdge() { return encSwitch.FallingEdge(); }

  bool Pressed() { return encSwitch.Pressed(); }

  float TimeHeldMs() { return encSwitch.TimeHeldMs(); }

private:
  Switch encSwitch;
  uint8_t a_, b_, pinA_, pinB_;
  int32_t inc_;
};
} // namespace daisy
#endif