/* CV_Input.ino

author: beserge

This example shows how to use the Patch SM's bipolar CV inputs.
When the input receives negative voltage, it turns the built in led off.
If the input receives positive voltage, it turns the built in led on.

To test this you can connect a jack to CV IN 5 on your Patch SM, and connect a voltage source to that jack.
Check the figures from the Patch SM datasheet for help connecting peripherals.
You can also use the patch.Init().
*/

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
    /** Update the control ins */
    patch.ProcessAllControls();

    /** Read from CV_1 */
    int value = analogRead(PIN_PATCH_SM_CV_5);

    /** Convert AnalogRead range to volts */
    float volts = patch.AnalogReadToVolts(value);

    /** If the Adc value is greater than 0V... */
    if(volts > 0)
    {
        /** Turn the led on */
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        /** Otherwise, turn the led off */
        digitalWrite(LED_BUILTIN, LOW);
    }
}