
#include "DaisyDuino.h"

DaisyHardware hw;
Oscillator osc, lfo;
SampleRateReducer sr;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    sr.SetFreq(fabsf(lfo.Process()));
    out[0][i] = out[1][i] = sr.Process(osc.Process());
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);
  osc.SetFreq(440.f);

  lfo.Init(sample_rate);
  lfo.SetFreq(.1f);
  lfo.SetAmp(.25f);

  sr.Init();

  DAISY.begin(AudioCallback);
}

void loop() {}
