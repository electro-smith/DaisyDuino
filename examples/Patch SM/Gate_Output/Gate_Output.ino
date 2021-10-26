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