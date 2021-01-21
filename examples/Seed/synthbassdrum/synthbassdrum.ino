
#include "DaisyDuino.h"

DaisyHardware hw;
SyntheticBassDrum bd;
Metro tick;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float t = tick.Process();
    if (t) {
      bd.SetAccent(random() / (float)RAND_MAX);
      bd.SetDirtiness(random() / (float)RAND_MAX);
      bd.SetDecay(random() / (float)RAND_MAX);
    }
    out[0][i] = out[1][i] = bd.Process(t);
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  bd.Init(sample_rate);
  bd.SetFreq(50.f);
  bd.SetDirtiness(.5f);
  bd.SetFmEnvelopeAmount(.6f);

  tick.Init(2.f, sample_rate);

  DAISY.begin(AudioCallback);
}

void loop() {}
