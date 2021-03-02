#pragma once
#ifndef DSY_KNOB_H
#define DSY_KNOB_H /**< & */
#include <stdint.h>
#include "Arduino.h"

#ifdef __cplusplus
namespace daisy
{
/**
    @brief Hardware Interface for control inputs \n 
    Primarily designed for ADC input controls such as \n 
    potentiometers, and control voltage. \n 
    @author Stephen Hensley
    @date November 2019
    @ingroup controls
*/
class AnalogControl
{
  public:
    /** Constructor */
    AnalogControl() {}
    /** destructor */
    ~AnalogControl() {}

    /** 
    Initializes the control
	\param pin arduino pin number the knob is attached to
    \param sr is the samplerate in Hz that the Process function will be called at.
    \param flip determines whether the input is flipped (i.e. 1.f - input) or not before being processed.1
    \param invert determines whether the input is inverted (i.e. -1.f * input) or note before being processed.
    \param slew_seconds is the slew time in seconds that it takes for the control to change to a new value.
    */
    void Init(uint8_t pin,
              float     sr,
              bool      flip         = false,
              bool      invert       = false,
              float     slew_seconds = 0.002f);

    /** 
    This Initializes the AnalogControl for a -5V to 5V inverted input
    All of the Init details are the same otherwise
    \param pin Arduino pin the knob is attached to
    \param sr Audio engine sample rate
    */
    void InitBipolarCv(uint8_t pin, float sr);

    /** 
    Filters, and transforms a raw ADC read into a normalized range.
    this should be called at the rate of specified by samplerate at Init time.   
    Default Initializations will return 0.0 -> 1.0
    Bi-polar CV inputs will return -1.0 -> 1.0
    */
    float Process();

    /** Returns the current stored value, without reprocessing */
    inline float Value() const { return val_; }

  private:
  
    uint8_t pin_;
    float     coeff_, samplerate_, val_;
	float frac_ = 1.f / 1023.f;
    float     scale_, offset_;
    bool      flip_;
    bool      invert_;
};
} // namespace daisy
#endif
#endif