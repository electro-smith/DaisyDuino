#include "DaisyAudio.h"

Pod pod;

uint8_t color;

void setup()
{
    DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    pod.Init(1000);

    color = 0;

    //setup I/O
    pinMode(PIN_POD_LED_1_RED, OUTPUT);
    pinMode(PIN_POD_LED_1_GREEN, OUTPUT);
    pinMode(PIN_POD_LED_1_BLUE, OUTPUT);

    pinMode(PIN_POD_LED_2_RED, OUTPUT);
    pinMode(PIN_POD_LED_2_GREEN, OUTPUT);
    pinMode(PIN_POD_LED_2_BLUE, OUTPUT);

    pinMode(PIN_POD_ENC_CLICK, INPUT_PULLUP);
    pinMode(PIN_POD_ENC_A, INPUT_PULLUP);
    pinMode(PIN_POD_ENC_B, INPUT_PULLUP);
}

void SetColor()
{
    digitalWrite(PIN_POD_LED_1_RED, (color & B100) != B100);
    digitalWrite(PIN_POD_LED_1_GREEN, (color & B010) != B010);
    digitalWrite(PIN_POD_LED_1_BLUE, (color & B001) != B001);

    digitalWrite(PIN_POD_LED_2_RED, (color & B100) != B100);
    digitalWrite(PIN_POD_LED_2_GREEN, (color & B010) != B010);
    digitalWrite(PIN_POD_LED_2_BLUE, (color & B001) != B001);
}

void loop()
{
    pod.encoder.ProcessClick(digitalRead(PIN_POD_ENC_CLICK));
    pod.encoder.ProcessInc(digitalRead(PIN_POD_ENC_A), digitalRead(PIN_POD_ENC_B));

    if (pod.encoder.RisingEdge())
    {
        color = 0;
    }

    color += pod.encoder.Increment();
    color = (color % 8 + 8) % 8;

    SetColor();
}