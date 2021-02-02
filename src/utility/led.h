#ifndef DSY_LED_H
#define DSY_LED_H

namespace daisy {
class Led {
public:
  Led() {}
  ~Led() {}

  void Init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b);
  
  // no PWM for now
  void Set(bool r, bool g, bool b);
  
private:
  uint8_t pin_r_, pin_g_, pin_b_;
};
} // namespace daisy