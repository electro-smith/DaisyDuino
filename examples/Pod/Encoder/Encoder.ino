#include "DaisyAudio.h"

DaisyPod pod;

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
    pod.Debounce();

    if (pod.encoder.RisingEdge())
    {
        color = 0;
    }

    color += pod.encoder.Increment();
    color = (color % 8 + 8) % 8;

    SetColor();
}