#ifndef DSY_DSYDUINO_H
#define DSY_DSYDUINO_H

#include <stdio.h>
#include "Arduino.h"
#include "DaisyDSP.h"
#include "daisy_pod.h"
#include "daisy_patch.h"
#include "AudioClass.h"

#include "utility/encoder.h"
#include "utility/gatein.h"
#include "utility/led.h"
#include "utility/switch.h"


using namespace daisy;

enum DaisyDuinoDevice : short{
    DAISY_SEED,
    DAISY_POD,
    DAISY_PETAL,
    DAISY_FIELD,
    DAISY_PATCH,
    DAISY_LAST,
};

class AudioClass; ///forward declaration

class DaisyHardware
{
public:
	DaisyHardware() {}

    Switch buttons[2];
    Encoder encoder;
    Led leds[2];
    GateIn gateIns[2];
    
    int num_channels;
    int numSwitches, numLeds, numGates;

    void Init(float control_update_rate, DaisyDuinoDevice device);

    void DebounceControls();

private:
    DaisyDuinoDevice device_;
};

#endif
