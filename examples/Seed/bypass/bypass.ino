#include "DaisyAudio.h"


size_t num_channels;

void MyCallback(float **in, float **out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t chn = 0; chn < num_channels; chn++)
        {
            out[chn][i] = in[chn][i];
        }
    }
}

void setup() {
    float samplerate;
    // Initialize for Daisy pod at 48kHz
    num_channels = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    samplerate = DAISY.get_samplerate();

    DAISY.begin(MyCallback);
}

void loop() {
}
