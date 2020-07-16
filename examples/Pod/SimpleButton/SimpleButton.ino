#include "DaisyAudio.h"

DaisyHardware hw;

bool led2on;

void setup()
{
    led2on = false;
    hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);
}

void loop()
{
    delay(1);
    hw.DebounceControls();
     
    // using button1 as momentary switch for turning on/off led1
    bool onePressed = hw.buttons[0].Pressed();
    hw.leds[0].Set(onePressed, onePressed, onePressed);
 
    // using button2 as latching switch for toggling led2
    if(hw.buttons[1].RisingEdge())
        led2on = !led2on;

    hw.leds[1].Set(led2on, led2on, led2on);
}