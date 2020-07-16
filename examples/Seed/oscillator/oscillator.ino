#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc;

void MyCallback(float **in, float **out, size_t size)
{
    // Convert Pitchknob MIDI Note Number to frequency
    osc.SetFreq(mtof(pitchknob));
    for (size_t i = 0; i < size; i++)
    {
        float sig = osc.Process();

        for (size_t chn = 0; chn < num_channels; chn++)
        {
            out[chn][i] = sig;
        }
    }
}

void setup() {
    float sample_rate;
    // Initialize for Daisy pod at 48kHz
    hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
    num_channels = hw.num_channels;
    sample_rate = DAISY.get_samplerate();

    osc.Init(sample_rate);
    osc.SetFreq(440);
    osc.SetAmp(0.5);
    osc.SetWaveform(osc.WAVE_TRI);


    DAISY.begin(MyCallback);
}

void loop()
{
    pitchknob = 24.0 + ((analogRead(A0) / 1023.0) * 60.0);
}
