#include "encoder.h"

using namespace daisy;

void Encoder::Init(float update_rate, uint8_t pinA, uint8_t pinB,
                   uint8_t pinClick, uint8_t modeA, uint8_t modeB,
                   uint8_t modeC) {
  inc_ = 0;
  a_ = b_ = 0xff;

  pinA_ = pinA;
  pinB_ = pinB;

  pinMode(pinA, modeA);
  pinMode(pinB, modeB);

  encSwitch.Init(update_rate, true, pinClick, modeC);
}

void Encoder::Debounce() {
  uint8_t a_in = digitalRead(pinA_);
  uint8_t b_in = digitalRead(pinB_);

  encSwitch.Debounce();

  a_ = (a_ << 1) | (a_in);
  b_ = (b_ << 1) | (b_in);

  inc_ = 0;

  if ((a_ & 0x0f) == 0x0e && (b_ & 0x07) == 0x00) {
    inc_ = 1;
  } else if ((b_ & 0x0f) == 0x0e && (a_ & 0x07) == 0x00) {
    inc_ = -1;
  }
}
