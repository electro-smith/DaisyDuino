#include "DaisyDuino.h"

Switch button;

void setup()
{
    //setup the button
    //update at 1kHz, no invert, on pin 28
    button.Init(1000, true, 28);

    pinMode(LED_BUILTIN, OUTPUT);
}
void loop()
{
    //Debounce the button
    button.Debounce();

    //If the button is pressed, turn the LED on
    digitalWrite(LED_BUILTIN, button.Pressed());

    //wait 1 ms
    delay(1);
}