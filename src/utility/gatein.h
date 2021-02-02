#ifndef DSY_GATEIN_H
#define DSY_GATEIN_H

#include "switch.h"
#include "Arduino.h"

namespace daisy {
class GateIn {
public:
  GateIn() {}
  ~GateIn() {}

  void Init(uint8_t pin, uint8_t mode, bool invert) {
    sw.Init(1, invert, pin, mode);
  }

  bool State() { return sw.Pressed(); }

  bool Trig() { return sw.RisingEdge(); }

  void Debounce() { sw.Debounce(); }

private:
  Switch sw;
};
} // namespace daisy
#endif