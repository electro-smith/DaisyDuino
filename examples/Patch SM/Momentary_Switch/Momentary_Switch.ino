/** Momentary_Switch.ino

author: beserge

This example shows how to use a momentary switch with the Patch SM.
When the button is held down the onboard led will be lit up.

To test this, connect a momentary switch to the Patch SM's pin B7.
Check the figures from the Patch SM datasheet for help connecting peripherals.
You can also use the momentary switch on the patch.Init()
*/

#include "DaisyDuino.h"

DaisyHardware patch;
Switch button;

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
    button.Init(1000, true, PIN_PATCH_SM_B7, INPUT_PULLUP);

    /** Set the builtin led pin to be an output */
    pinMode(LED_BUILTIN, OUTPUT);
}

/** loop forever */
void loop()
{
    /** Debounce the switch */
    button.Debounce();

    /** Get the current button state */
    bool state = button.Pressed();

    /** Set the onboard led to the current state */
    digitalWrite(LED_BUILTIN, state);
}