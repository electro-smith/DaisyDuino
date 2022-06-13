#pragma once
#include "Arduino.h"
#include "daisy_core.h"

namespace daisy
{
class Switch3
{
  public:
    enum
    {
        POS_CENTER = 0,
        POS_LEFT   = 1,
        POS_UP     = 1,
        POS_RIGHT  = 2,
        POS_DOWN   = 2,
    };


    Switch3() {}
    ~Switch3() {}

    void Init(uint8_t pina, uint8_t pinb)
    {
        pina_ = pina;
        pinb_ = pinb;
        pinMode(pina, INPUT_PULLUP);
        pinMode(pinb, INPUT_PULLUP);
    }

    int Read()
    {
        if(!digitalRead(pina_))
            return POS_UP;
        if(!digitalRead(pinb_))
            return POS_DOWN;
        return POS_CENTER;
    }

  private:
    uint8_t pina_;
    uint8_t pinb_;
};

} // namespace daisy