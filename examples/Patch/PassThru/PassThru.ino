#include "DaisyDuino.h"

size_t num_channels;

DaisyHardware hw;

void MyCallback(float **in, float **out, size_t size)
{
    float *in_left, *in_right, *out_left, *out_right;
    in_left = in[0];
    in_right = in[1];
    out_left = out[0];
    out_right = out[1];

    for (size_t i = 0; i < size; i++)
    {
        out_left[i] = in_left[i];
        out_right[i] = in_right[i];
    }
}

void setup()
{
    // Initialize for Daisy pod at 48kHz
    hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
    num_channels = hw.num_channels;
    pinMode(LED_BUILTIN, OUTPUT);
    DAISY.begin(MyCallback);
    // Debug Stuff for now
    Serial.begin(9600);
}

void loop()
{
    // Do something else here like blink the LED
    delay(250);
    digitalToggle(LED_BUILTIN);
    Serial.println(num_channels);
}
