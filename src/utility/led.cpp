#include "led.h"
#define RESOLUTION_MAX (65535)
#include <math.h>

using namespace daisy;

void Led::Init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert) {
  invert_ = invert;

  pin_r_ = pin_r;
  pin_g_ = pin_g;
  pin_b_ = pin_b;

  pinMode(pin_r, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_b, OUTPUT);
}

void Led::Init(uint8_t pin, bool invert) {
  invert_ = invert;
  pin_ = pin;
  pinMode(pin, OUTPUT);
}

void Led::Set(bool r, bool g, bool b){
	digitalWrite(pin_r_, invert_ ^ r);
	digitalWrite(pin_g_, invert_ ^ g);
	digitalWrite(pin_b_, invert_ ^ b);
}

void Led::Set(bool set){
	digitalWrite(pin_, invert_ ^ set);	
}


/**    This code will be returned to when PWM is addressed in libdaisy

void Led::Init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert, float samplerate) {
  pin_r_ = pin_r;
  pin_g_ = pin_g;
  pin_b_ = pin_b;
  pin_ = -1; //tells update we're in RGB mode

  invert_ = invert;
  samplerate_ = samplerate;

  pinMode(pin_r, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_b, OUTPUT);
}

void Led::Init(uint8_t pin) {
  pin_ = pin;
  pinMode(pin_, OUTPUT);
}



void Led::Set(float brightness) {
  bright_     = cbrt(brightness);
}

void Led::Set(float r, float g, float b) {
  bright_r_     = cbrt(r);
  bright_g_     = cbrt(g);
  bright_b_     = cbrt(b);
}

void Led::Update(){

    pwm_ += 120.f / samplerate_;
    if(pwm_ > 1.f)
        pwm_ -= 1.f;
    	
	digitalWrite(pin_r_, invert_ ^ (bright_r_ > pwm_));
	digitalWrite(pin_g_, invert_ ^ (bright_g_ > pwm_));
	digitalWrite(pin_b_, invert_ ^ (bright_b_ > pwm_));
}
**/