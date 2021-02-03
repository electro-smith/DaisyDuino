#ifndef DSY_DSYDUINO_H
#define DSY_DSYDUINO_H

#include <stdio.h>
#include "Arduino.h"

#include "AudioClass.h"
#include "DaisyDSP.h"
#include "daisy_patch.h"
#include "daisy_pod.h"
#include "daisy_petal.h"

#include "utility/encoder.h"
#include "utility/gatein.h"
#include "utility/i2c.h"
#include "utility/led.h"
#include "utility/led_driver.h"
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
  Switch* switches = buttons; //looks better for petal
  Encoder encoder;
  Led leds[2];
  GateIn gateIns[2];

  int num_channels;
  int numSwitches, numLeds, numGates;

  void Init(float control_update_rate, DaisyDuinoDevice device);

  //set ring led color. For use with daisy petal
  void SetRingLed(uint8_t idx, float r, float g, float b);

  //Set footswitch LED. For use with daisy petal only
  void SetFootswitchLed(uint8_t idx, float bright);

  //use with petal and field
  void ClearLeds();

  //use with petal and field
  void UpdateLeds();

  void DebounceControls();

private:
  void InitPod(float control_update_rate);
  void InitPatch(float control_update_rate);
  void InitPetal(float control_update_rate);

  LedDriverPca9685<2, true> led_driver_;

  DaisyDuinoDevice device_;
};

#endif
