#ifndef DSY_LED_H
#define DSY_LED_H

#include "Arduino.h"

namespace daisy {
class Led {
public:
  Led() {}
  ~Led() {}

  //RGB leds
  void Init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert, float samplerate);
  void Set(float r, float g, float b);
  
  void Init(uint8_t pin);
  void Set(float brightness);
  
  void Update();
  
private:
  uint8_t pin_r_, pin_g_, pin_b_;
  uint8_t pin_;
  
  float bright_r_;
  float bright_g_;
  float bright_b_;
  float bright_;
  
  bool invert_;
  float samplerate_;
  
  float pwm_;
};
} // namespace daisy
#endif