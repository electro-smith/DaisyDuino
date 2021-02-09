#include "ctrl.h"
#include <math.h>
// Temporary amount to prevent bleed on the bottom of the pots/CVs
#define BOTTOM_THRESH 0.002f
using namespace daisy;

void AnalogControl::Init(uint8_t pin,
                         float     sr,
                         bool      flip,
                         bool      invert,
                         float     slew_seconds)
{
	pinMode(pin, INPUT);
	pin_ = pin;
	
	mux_ = false;
	
    val_        = 0.0f;
    samplerate_ = sr;
    coeff_      = 1.0f / (slew_seconds * samplerate_ * 0.5f);
    scale_      = 1.0f;
    offset_     = 0.0f;
    flip_       = flip;
    invert_     = invert;
}

void AnalogControl::InitMux(uint16_t* adcptr,
                         float     sr,
                         bool      flip,
                         bool      invert,
                         float     slew_seconds)
{
	mux_ = true;
	
	val_        = 0.0f;
    raw_        = adcptr;
    samplerate_ = sr;
    coeff_      = 1.0f / (slew_seconds * samplerate_ * 0.5f);
    scale_      = 1.0f;
    offset_     = 0.0f;
    flip_       = flip;
    invert_     = invert;
}


void AnalogControl::InitBipolarCv(uint8_t pin, float sr)
{
	pinMode(pin, INPUT);
	pin_ = pin;
	
    val_        = 0.0f;
    samplerate_ = sr;
    coeff_      = 1.0f / (0.002f * samplerate_ * 0.5f);
    scale_      = 2.0f;
    offset_     = 0.5f;
    flip_       = false;
    invert_     = true;
}

float AnalogControl::Process()
{
    float t;

	if(mux_){
		t = (float)*raw_ / 65536.f;
	}
	else{
		t = (float)analogRead(pin_) / 1023.f;
	}

    if(flip_)
        t = 1.f - t;
    t = (t - offset_) * scale_ * (invert_ ? -1.0f : 1.0f);
    val_ += coeff_ * (t - val_);
    return val_;
}