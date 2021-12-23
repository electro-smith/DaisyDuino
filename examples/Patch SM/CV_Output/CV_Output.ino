/* CV_Output.ino

author: beserge

This example shows how to use the Patch SM's CV outputs.
Stepped voltage is output on the CV OUT 2 pin.
This cycles through 0V, 2.5V, 5V stepping ahead once per second.

To test this example, connect an led to CV OUT 2 on your Patch SM.
Check the figures from the Patch SM datasheet for help connecting peripherals.
On the patch.Init() this is connected to the front panel led.
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
    /** Write 0V to CV_OUT_2 */
    patch.WriteCvOut(PIN_PATCH_SM_CV_OUT_2, 0);

    /** Wait for 1000 ms (1 second) */
    delay(1000);

    /** Write 2.5V to CV_OUT_2 */
    patch.WriteCvOut(PIN_PATCH_SM_CV_OUT_2, 2.5);

    /** Wait for 1000 ms (1 second) */
    delay(1000);

    /** Write 5V to CV_OUT_2 */
    patch.WriteCvOut(PIN_PATCH_SM_CV_OUT_2, 5);

    /** Wait for 1000 ms (1 second) */
    delay(1000);
}