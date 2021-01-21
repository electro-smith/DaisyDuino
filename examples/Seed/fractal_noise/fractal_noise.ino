
#include "DaisyDuino.h"

DaisyHardware hw;

FractalRandomGenerator<ClockedNoise, 5> fract;
Oscillator lfo[2];

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    fract.SetFreq(fabsf(lfo[0].Process()));
    fract.SetColor(fabsf(lfo[1].Process()));
    out[0][i] = out[1][i] = fract.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  fract.Init(sample_rate);
  fract.SetFreq(sample_rate / 10.f);

  lfo[0].Init(sample_rate);
  lfo[1].Init(sample_rate);

  lfo[0].SetFreq(.25f);
  lfo[0].SetAmp(sample_rate / 3.f);
  lfo[1].SetFreq(.1f);
  lfo[1].SetAmp(1.f);

  DAISY.begin(AudioCallback);
}

void loop() {}
