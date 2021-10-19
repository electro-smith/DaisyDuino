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
    analogWrite(PIN_PATCH_SM_CV_OUT_2, 0);

    /** Wait for 1000 ms (1 second) */
    delay(1000);

    /** Write 2.5V to CV_OUT_2 */
    analogWrite(PIN_PATCH_SM_CV_OUT_2, 128);

    /** Wait for 1000 ms (1 second) */
    delay(1000);

    /** Write 5V to CV_OUT_2 */
    analogWrite(PIN_PATCH_SM_CV_OUT_2, 255);

    /** Wait for 1000 ms (1 second) */
    delay(1000);
}