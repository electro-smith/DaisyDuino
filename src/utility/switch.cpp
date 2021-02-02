#include "switch.h"

using namespace daisy;

void Switch::Init(float update_rate, bool invert, uint8_t pin, uint8_t mode) {
  flip_ = invert;
  time_per_update_ = 1.f / update_rate;
  state_ = 0;
  time_held_ = 0.f;
  pin_ = pin;
  pinMode(pin, mode);
}

// debounces and processes input
void Switch::Debounce() {
  uint8_t in = digitalRead(pin_);
  state_ = (state_ << 1) | (flip_ ? !in : in);

  if (state_ == 0x7f || state_ == 0x80)
    time_held_ = 0;

  if (state_ == 0xff)
    time_held_ += time_per_update_;
}