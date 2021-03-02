#ifndef DSY_LED_H
#define DSY_LED_H

#include "Arduino.h"

namespace daisy {
class Led {
public:
  Led() {}
  ~Led() {}

  //RGB leds
  //invert default allows for bwd compatability
  void Init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert = true);
  void Set(bool r, bool g, bool b);
  
  //single leds
  void Init(uint8_t pin, bool invert = true);
  void Set(bool set);
    
private:
  uint8_t pin_r_, pin_g_, pin_b_;
  uint8_t pin_;
  
  bool invert_;
};
} // namespace daisy
#endif