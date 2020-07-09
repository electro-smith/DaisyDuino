#include "DaisyAudio.h"


size_t num_channels;

static WhiteNoise nse;

void MyCallback(float **in, float **out, size_t size)
{
    float sig;
    for (size_t i = 0; i < size; i++)
    {
       sig = nse.Process();

       for (size_t chn = 0; chn < num_channels; chn++)
       {
            out[chn][i] = sig;
       }
    }
}

void setup() {
    float sample_rate;
    // Initialize for Daisy pod at 48kHz
    num_channels = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    sample_rate = DAISY.get_samplerate();

    nse.Init();

    DAISY.begin(MyCallback);
}

void loop() {
}
