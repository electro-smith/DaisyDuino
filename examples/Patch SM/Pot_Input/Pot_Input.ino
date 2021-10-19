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

    /** Get CV_1 Input (512, 0) */
    int value = analogRead(PIN_PATCH_SM_CV_1);

    /** Flip the range (512, 0) -> (0, 512) */
    value = 512 - value;

    /** Here the pot is wired to GND and 5V
        * So 2.5V is the pot's halfway point.
        * 
        * If the Adc value is greater than 256 (512/2)... 
    */
    if(value > 256)
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