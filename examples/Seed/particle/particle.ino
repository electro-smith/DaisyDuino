
#include "DaisyDuino.h"

DaisyHardware hw;
Particle particle;
Oscillator lfo;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    particle.SetDensity(fabsf(lfo.Process()));
    out[0][i] = out[1][i] = particle.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  lfo.Init(sample_rate);
  lfo.SetAmp(.5f);
  lfo.SetFreq(.1f);

  particle.Init(sample_rate);
  particle.SetSpread(2.f);

  DAISY.begin(AudioCallback);
}

void loop() {}
