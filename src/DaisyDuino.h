#ifndef DSY_DSYDUINO_H
#define DSY_DSYDUINO_H

#include <stdio.h>
#include "Arduino.h"

#include "AudioClass.h"
#include "DaisyDSP.h"
#include "daisy_field.h"
#include "daisy_patch.h"
#include "daisy_pod.h"
#include "daisy_petal.h"

#include "utility/adc.h"

#include "utility/ctrl.h"
#include "utility/encoder.h"
#include "utility/gatein.h"
#include "utility/led.h"
#include "utility/led_driver.h"
#include "utility/parameter.h"
#include "utility/switch.h"

using namespace daisy;

enum DaisyDuinoDevice : short {
  DAISY_SEED,
  DAISY_POD,
  DAISY_PETAL,
  DAISY_FIELD,
  DAISY_PATCH,
  DAISY_LAST,
};

class AudioClass; /// forward declaration

class DaisyHardware {
public:
  DaisyHardware() {}

  Switch buttons[7];
  AnalogControl controls[8];
  AnalogControl cv[4]; //for use with field
  
  //runs the field pot MUX for the time being
  AdcHandle adc;
  
//  Switch* switches = buttons; //these compile, but don't actually work....
//  AnalogControl* knobs = controls;
  Encoder encoder;
  Led leds[2];
  GateIn gateIns[2];
  AnalogControl expression;

  int num_channels;
  int numSwitches, numLeds, numGates, numControls;

  void Init(float control_update_rate, DaisyDuinoDevice device);

  //set ring led color. For use with daisy petal
  void SetRingLed(uint8_t idx, float r, float g, float b);

  //Set footswitch LED. For use with daisy petal only
  void SetFootswitchLed(uint8_t idx, float bright);

  //use with petal and field
  void ClearLeds();

  //use with petal and field
  void UpdateLeds();

  //process knobs
  void ProcessAnalogControls();
  
  //process buttons and encoders
  void ProcessDigitalControls();
  
  //for backwards compatability
  void DebounceControls() { ProcessDigitalControls(); }
  
  //process boths
  void ProcessAllControls();

private:
  void InitPod(float control_update_rate);
  void InitPatch(float control_update_rate);
  void InitPetal(float control_update_rate);
  void InitField(float control_update_rate);

  LedDriverPca9685<2, true> led_driver_;

  DaisyDuinoDevice device_;
};

#endif
