/* Gate_Output.ino

author: beserge

This example demonstrates using the Patch SM's Gate Outputs.
The example simply toggles the gate output on and off four times a second.

To test this example, connect a jack to the Patch SM's Gate Out 1.
Patch that jack to any module accepting a gate input.
Check the figures from the Patch SM datasheet for help connecting peripherals.
You can also use gate_out_2 on the patch.Init()

*/

#include "DaisyDuino.h"

DaisyHardware patch;

/** Start here */
void setup()
{
    /** Initialize the patch_sm hardware object */
    patch = DAISY.init(DAISY_PATCH_SM);
}

/** Loop forever */
void loop()
{
    /** Set the gate high */
    digitalWrite(PIN_PATCH_SM_GATE_OUT_1, HIGH);

    /** Wait 250 ms */
    delay(250);

    /** Set the gate low */
    digitalWrite(PIN_PATCH_SM_GATE_OUT_1, LOW);

    /** Wait 250 ms */
    delay(250);
}