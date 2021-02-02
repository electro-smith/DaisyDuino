#include "led.h"

using namespace daisy;

void Led::Init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b) {
  pin_r_ = pin_r;
  pin_g_ = pin_g;
  pin_b_ = pin_b;

  pinMode(pin_r, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_b, OUTPUT);
}

// no PWM for now
void Led::Set(bool r, bool g, bool b) {
  digitalWrite(pin_r_, !r);
  digitalWrite(pin_g_, !g);
  digitalWrite(pin_b_, !b);
}
