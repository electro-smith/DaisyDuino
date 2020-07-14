#include "DaisyAudio.h"

DaisyPod pod;

uint8_t color;

void setup()
{
    DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    pod.Init(1000);

    color = 0;
}

void SetColor()
{
    pod.led1.Set((color & B100) == B100, (color & B010) == B010, (color & B001) == B001);
    pod.led2.Set((color & B100) == B100, (color & B010) == B010, (color & B001) == B001);
}

void loop()
{
    pod.DebounceControls();

    if (pod.encoder.RisingEdge())
    {
        color = 0;
    }

    color += pod.encoder.Increment();
    color = (color % 8 + 8) % 8;

    SetColor();
}