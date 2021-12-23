/* Pot_Input.ino

author: beserge

This example shows how to use a potentiometer with the Patch SM.
Turning the pot above its halfway point turns on the builtin led.
Turning the pot below its halfway point turns off the builtin led.

To test this, connect a pot to pin CV 1 on your Patch SM.
Check the figures from the Patch SM datasheet for help connecting peripherals.
You can also use the CV_1 pot on the patch.Init()

*/

#include "DaisyDuino.h"

DaisyHardware patch;

void setup()
{
    /** Initialize the patch_sm hardware object */
    patch = DAISY.init(DAISY_PATCH_SM);

    /** Set the builtin led pin as an output */
    pinMode(LED_BUILTIN, OUTPUT);
}

/** Loop forever */
void loop()
{
    /** Update all inputs */
    patch.ProcessAllControls();

    /** Get CV_1 Input */
    int value = analogRead(PIN_PATCH_SM_CV_1);

    /** Convert the AnalogRead range to volts */
    float volts = patch.AnalogReadToVolts(value);

    /** Here the pot is wired to GND and 5V
        * So 2.5V is the pot's halfway point.
        * 
        * If the Adc value is greater than 2.5 V... 
    */
    if(volts > 2.5)
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