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