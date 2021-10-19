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

    /** Read from CV_1 (1024, 0) */
    float value = analogRead(PIN_PATCH_SM_CV_5);

    /** Flip range (1024, 0) -> (0, 1024) */
    value = 1024 - value;

    /** Convert to volts (0, 1024) -> (0V, 10V) */
    value /= 102.4;

    /** Offset (0V, 10V) -> (-5V, 5V) */
    value -= 5;

    /** If the Adc value is greater than 0V... */
    if(value > 0)
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