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