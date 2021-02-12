
#include "DaisyDuino.h"

DaisyHardware hw;
HiHat<> hihat;
Metro tick;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    bool t = tick.Process();
    if (t) {
      hihat.SetDecay(random() / (float)RAND_MAX);
      hihat.SetSustain((random() / (float)RAND_MAX) > .8f);
      hihat.SetTone(random() / (float)RAND_MAX);
      hihat.SetNoisiness(random() / (float)RAND_MAX);
    }
    out[0][i] = out[1][i] = hihat.Process(t);
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  hihat.Init(sample_rate);

  tick.Init(8.f, sample_rate);

  DAISY.begin(AudioCallback);
}

void loop() {}
