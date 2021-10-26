#include "DaisyDuino.h"

DaisyHardware patch;
Switch toggle;

/** Start here */
void setup()
{
    /** Initialize the patch_sm hardware object */
    patch = DAISY.init(DAISY_PATCH_SM);

    /* Initialize the switch
     - Set to 1khz update rate
     - Invert switch signal
	 - We'll read the switch on pin B7
     - Set the input pin as a pullup
    */
    toggle.Init(1000, true, PIN_PATCH_SM_B8, INPUT_PULLUP);

    /** Set the builtin led pin to an output */
    pinMode(LED_BUILTIN, OUTPUT);
}

/** Loop forever */
void loop()
{
    /** Debounce the switch */
    toggle.Debounce();

    /** Get the current toggle state */
    bool state = toggle.Pressed();

    /** Set the onboard led to the current state */
    digitalWrite(LED_BUILTIN, state);
}