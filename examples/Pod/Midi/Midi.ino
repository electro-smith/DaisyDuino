#include "DaisyDuino.h"

DaisyHardware hw;

Oscillator osc;

static void AudioCallback(float **in, float **out, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        out[0][i] = out[1][i] = osc.Process();
    }
}

void setup()
{
    hw = DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    float samplerate = hw.get_samplerate();

    osc.Init(samplerate);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetFreq(440);

    DAISY.begin(AudioCallback);
}

void loop()
{
}