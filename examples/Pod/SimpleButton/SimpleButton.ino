#include "DaisyAudio.h"

DaisyPod hw;

bool led2on;

void setup()
{
    led2on = false;
    hw.Init(1000);

    pinMode(PIN_POD_LED_1_RED, OUTPUT);
    pinMode(PIN_POD_LED_1_GREEN, OUTPUT);
    pinMode(PIN_POD_LED_1_BLUE, OUTPUT);

    pinMode(PIN_POD_LED_2_RED, OUTPUT);
    pinMode(PIN_POD_LED_2_GREEN, OUTPUT);
    pinMode(PIN_POD_LED_2_BLUE, OUTPUT);
}

void loop()
{
    delay(1);
    hw.Debounce();
     
    // using button1 as momentary switch for turning on/off led1
    //pod leds are active low
    digitalWrite(PIN_POD_LED_1_RED, !hw.button1.Pressed());
    digitalWrite(PIN_POD_LED_1_GREEN, !hw.button1.Pressed());
    digitalWrite(PIN_POD_LED_1_BLUE, !hw.button1.Pressed());

    // using button2 as latching switch for toggling led2
    if(hw.button2.RisingEdge())
        led2on = !led2on;

    digitalWrite(PIN_POD_LED_2_RED, !led2on);
    digitalWrite(PIN_POD_LED_2_GREEN, !led2on);
    digitalWrite(PIN_POD_LED_2_BLUE, !led2on);
}