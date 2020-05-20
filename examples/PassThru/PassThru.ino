#include "AUDIO.h"

size_t num_channels;

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
    num_channels = AUDIO.init(DAISY_PATCH, AUDIO_SR_48K);
    pinMode(LED_BUILTIN, OUTPUT);
    AUDIO.begin(MyCallback);
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
