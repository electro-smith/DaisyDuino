
#include "DaisyDuino.h"

DaisyHardware hw;
SyntheticSnareDrum sd;
Metro tick;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    bool t = tick.Process();
    if (t) {
      sd.SetAccent(random() / (float)RAND_MAX);
      sd.SetDecay(random() / (float)RAND_MAX);
      sd.SetSnappy(random() / (float)RAND_MAX);
    }
    out[0][i] = out[1][i] = sd.Process(t);
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  tick.Init(2.f, sample_rate);

  sd.Init(sample_rate);

  DAISY.begin(AudioCallback);
}

void loop() {}
