#include "AUDIO.h"

int num_channels;

void MyCallback(float **in, float **out, uint32_t size)
{
    float *in_left, *in_right, *out_left, *out_right;
    in_left = in[0];
    in_right = in[1];
    out_left = out[0];
    out_right = out[1];

    for (int i = 0; i < size; i++)
    {
        out_left[i] = in_left[i];
        out_right[i] = in_right[i];
    }
}

void setup()
{
    // Initialize for Daisy pod at 48kHz
    num_channels = AUDIO.Init(DAISY_POD, 48000);
    Audio.Start(MyCallback);
}

void loop()
{
    // Do something else here like blink the LED
    delay(250);
    digitalToggle(LED_BUILTIN);
}
