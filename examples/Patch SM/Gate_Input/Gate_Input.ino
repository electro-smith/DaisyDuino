#include "DaisyDuino.h"

DaisyHardware patch;

/** Start here */
void setup()
{
    /** Initialize the patch_sm hardware object */
    patch = DAISY.init(DAISY_PATCH_SM);
    pinMode(LED_BUILTIN, OUTPUT);
}

/** Loop forever */
void loop()
{
    /** Process all inputs, including Gate Ins */
    patch.ProcessAllControls();
  
    /** Get the current gate in state */
    bool state = patch.gateIns[0].State();

    /** Set the led to the gate state */
    digitalWrite(LED_BUILTIN, state);
}