#include "DaisyDuino.h"

static DaisyHardware seed;
Fm2 osc;
Oscillator lfo1, lfo2, lfo3;

static void AudioCallback(float **in, float **out, size_t size) {
  float output;
  for (size_t i = 0; i < size; i++) {
    float lfo1out = lfo1.Process();
    float lfo2out = lfo2.Process();
    float lfo3out = lfo3.Process();

    osc.SetIndex(lfo1out);
    osc.SetRatio(lfo2out + 3);
    osc.SetFrequency((lfo3out + 1) * 220.f + 440.f);

    output = osc.Process();
    out[0][i] = out[1][i] = output;
  }
}

void setup() {
  // initialize seed hardware and daisysp modules
  float sample_rate;
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);
  lfo1.Init(sample_rate);
  lfo2.Init(sample_rate);
  lfo3.Init(sample_rate);

  lfo1.SetFreq(.05f);

  lfo2.SetFreq(.01f);
  lfo2.SetWaveform(Oscillator::WAVE_SQUARE);

  lfo3.SetFreq(.5f);
  lfo3.SetAmp(1);
  lfo3.SetWaveform(Oscillator::WAVE_SQUARE);

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {}
