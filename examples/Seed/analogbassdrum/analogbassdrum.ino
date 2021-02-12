// Title: analogbassdrum
// Description: Demonstrates AnalogBassDrum with randomized parameters.
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;
AnalogBassDrum bd;
Metro tick;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    bool t = tick.Process();
    if (t) {
      bd.SetTone(.7f * random() / (float)RAND_MAX);
      bd.SetDecay(random() / (float)RAND_MAX);
      bd.SetSelfFmAmount(random() / (float)RAND_MAX);
    }

    out[0][i] = out[1][i] = bd.Process(t);
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  float sample_rate = DAISY.get_samplerate();

  bd.Init(sample_rate);
  bd.SetFreq(50.f);

  tick.Init(2.f, sample_rate);

  DAISY.begin(AudioCallback);
}

void loop() {}
