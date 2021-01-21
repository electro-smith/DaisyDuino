
#include "DaisyDuino.h"

DaisyHardware hw;
ClockedNoise noise;
Oscillator lfo;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    noise.SetFreq(fabsf(lfo.Process()));
    out[0][i] = out[1][i] = noise.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  noise.Init(sample_rate);

  lfo.Init(sample_rate);
  lfo.SetAmp(sample_rate / 3.f);
  lfo.SetFreq(.125f);

  DAISY.begin(AudioCallback);
}

void loop() {}
