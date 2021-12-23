#ifndef DSY_DSYDUINO_H
#define DSY_DSYDUINO_H

#include "Arduino.h"
#include <Wire.h>
#include <stdio.h>

#include "AudioClass.h"
#include "DaisyDSP.h"
#include "daisy_field.h"
#include "daisy_patch.h"
#include "daisy_petal.h"
#include "daisy_pod.h"
#include "daisy_patch_sm.h"

#include "utility/ctrl.h"
#include "utility/encoder.h"
#include "utility/gatein.h"
#include "utility/led.h"
#include "utility/led_driver.h"
#include "utility/parameter.h"
#include "utility/sr_4021.h"
#include "utility/switch.h"

#define OUT_L out[0]
#define OUT_R out[1]
#define IN_L in[0]
#define IN_R in[1]

using namespace daisy;

enum DaisyDuinoDevice : short {
  DAISY_SEED,
  DAISY_POD,
  DAISY_PETAL,
  DAISY_FIELD,
  DAISY_PATCH,
  DAISY_PATCH_SM,
  DAISY_LAST,
};

class AudioClass; // forward declaration

class DaisyHardware {
public:
  DaisyHardware() {}

  Switch buttons[7];
  AnalogControl controls[12];
  AnalogControl cv[4]; // for use with field

  //  Switch* switches = buttons; //these compile, but don't actually work....
  //  AnalogControl* knobs = controls;
  Encoder encoder;
  Led leds[2];
  GateIn gateIns[2];
  AnalogControl expression;
  LedDriverPca9685<2, true> led_driver_; // public for now

  int num_channels;
  int numSwitches, numLeds, numGates, numControls;

  void Init(float control_update_rate, DaisyDuinoDevice device);

  float AnalogReadToVolts(int input){
    float ret = (512.f - (float)input) * .009765625f;
    return constrain(ret, 0, 1024);
  }

  int VoltsToAnalogWrite(float volts){
    return constrain(volts * 51.2f, 0, 255);
  }

  void WriteCvOut(uint8_t pin, float voltage){
    analogWrite(pin, VoltsToAnalogWrite(voltage));
  }

  //Gets the value from a control
  float GetAdcValue(size_t idx){
    if(idx < numControls){
      return controls[idx].Value();
    }
    return 0.f;
  }

  // set ring led color. For use with daisy petal
  void SetRingLed(uint8_t idx, float r, float g, float b);

  // Set footswitch LED. For use with daisy petal only
  void SetFootswitchLed(uint8_t idx, float bright);

  // Set keyboard LED. For use with daisy field only
  void SetKeyboardLed(uint8_t row, uint8_t idx, float value);

  // Set knob LED. For use with daisy field only
  void SetKnobLed(uint8_t idx, float bright);

  // use with petal and field
  void ClearLeds();

  // use with petal and field
  void UpdateLeds();

  // for field MUX knobs
  float GetKnobValue(uint8_t idx);

  bool KeyboardState(size_t idx);

  bool KeyboardRisingEdge(size_t idx);

  bool KeyboardFallingEdge(size_t idx);

  // process knobs
  void ProcessAnalogControls();

  // process buttons and encoders
  void ProcessDigitalControls();

  // for backwards compatability
  void DebounceControls() { ProcessDigitalControls(); }

  // process boths
  void ProcessAllControls();

private:
  void InitPod(float control_update_rate);
  void InitPatch(float control_update_rate);
  void InitPetal(float control_update_rate);
  void InitField(float control_update_rate);
  void InitPatchSM(float control_update_rate);

  DaisyDuinoDevice device_;

  // field keyboard
  ShiftRegister4021<2> keyboard_sr_; /**< Two 4021s daisy-chained. */
  uint8_t keyboard_state_[16];
};

#endif
