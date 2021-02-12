
#include "DaisyDuino.h"

DaisyHardware hw;
Dust dust;
Oscillator lfo;
void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    dust.SetDensity(fabsf(lfo.Process()));
    out[0][i] = out[1][i] = dust.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  dust.Init();
  lfo.Init(sample_rate);
  lfo.SetFreq(.1f);
  lfo.SetAmp(1.f);

  DAISY.begin(AudioCallback);
}

void loop() {}
