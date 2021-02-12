
#include "DaisyDuino.h"

DaisyHardware hw;

SmoothRandomGenerator smooth;
Oscillator osc;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    osc.SetFreq((fabsf(smooth.Process()) * 330.f) + 110.f);
    out[0][i] = out[1][i] = osc.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  smooth.Init(sample_rate);
  smooth.SetFreq(.75f);

  osc.Init(sample_rate);
  osc.SetFreq(440.f);

  DAISY.begin(AudioCallback);
}

void loop() {}
