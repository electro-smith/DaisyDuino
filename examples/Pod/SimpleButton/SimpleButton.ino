#include "DaisyAudio.h"

DaisyPod hw;

bool led2on;

void setup()
{
    led2on = false;
    hw.Init(1000);
}

void loop()
{
    delay(1);
    hw.DebounceControls();
     
    // using button1 as momentary switch for turning on/off led1
    bool onePressed = hw.button1.Pressed();
    hw.led1.Set(onePressed, onePressed, onePressed);
 
    // using button2 as latching switch for toggling led2
    if(hw.button2.RisingEdge())
        led2on = !led2on;

    hw.led2.Set(led2on, led2on, led2on);
}