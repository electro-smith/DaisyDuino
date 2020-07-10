#include "DaisyAudio.h"

Pod pod;

uint8_t color;

Encoder enc; //turn the encoder
Switch sw; //press the encoder

void setup()
{
    DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    pod.Init(1000);

    color = 0;

    //setup I/O
    for (int i = 0; i < 3; i++)
    {
        pinMode(pod.ledPins[0][i], OUTPUT);
        pinMode(pod.ledPins[1][i], OUTPUT);
    }

    pinMode(pod.encIncPins[0], INPUT_PULLUP);
    pinMode(pod.encIncPins[1], INPUT_PULLUP);
    pinMode(pod.encClickPin, INPUT_PULLUP);
}

void SetColor()
{
    digitalWrite(pod.ledPins[0][0], (color & B100) != B100);
    digitalWrite(pod.ledPins[0][1], (color & B010) != B010);
    digitalWrite(pod.ledPins[0][2], (color & B001) != B001);

    digitalWrite(pod.ledPins[1][0], (color & B100) != B100);
    digitalWrite(pod.ledPins[1][1], (color & B010) != B010);
    digitalWrite(pod.ledPins[1][2], (color & B001) != B001);
}

void loop()
{
    pod.encSwitch.Process(digitalRead(pod.encClickPin));
    pod.enc.Process(digitalRead(pod.encIncPins[0]), digitalRead(pod.encIncPins[1]));

    if (pod.encSwitch.RisingEdge())
    {
        color = 0;
    }

    color += pod.enc.Increment();
    color = (color % 8 + 8) % 8;

    SetColor();
}