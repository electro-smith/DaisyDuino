#include "DaisyAudio.h"

AudioClass::Pod pod;

uint8_t color;

void setup()
{
    DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    pod = DAISY.pod;

    color = 0;

    //setup I/O
    for (int i = 0; i < 3; i++)
    {
        pinMode(pod.leds[0][i], OUTPUT);
        pinMode(pod.leds[1][i], OUTPUT);
    }

    pinMode(pod.encoder_inc[0], INPUT_PULLUP);
    pinMode(pod.encoder_inc[1], INPUT_PULLUP);
    pinMode(pod.encoder_click, INPUT_PULLUP);
}

void SetColor()
{
    digitalWrite(pod.leds[0][0], (color & B100) != B100);
    digitalWrite(pod.leds[0][1], (color & B010) != B010);
    digitalWrite(pod.leds[0][2], (color & B001) != B001);

    digitalWrite(pod.leds[1][0], (color & B100) != B100);
    digitalWrite(pod.leds[1][1], (color & B010) != B010);
    digitalWrite(pod.leds[1][2], (color & B001) != B001);
}

void loop()
{
    if (! digitalRead(pod.encoder_click))
    {
        color = 0;
    }

    else if (! digitalRead(pod.encoder_inc[0]))
    {
        color++;
        color %= 8;
    }

    SetColor();
}