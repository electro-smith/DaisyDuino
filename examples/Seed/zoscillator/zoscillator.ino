
#include "DaisyDuino.h"

DaisyHardware hw;
ZOscillator zosc;
Oscillator lfo1, lfo2;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float mod1 = lfo1.Process();
    float mod2 = lfo2.Process();

    zosc.SetFormantFreq(250.f * (1.f + mod1 * .8f));
    zosc.SetMode(mod2);

    out[0][i] = out[1][i] = zosc.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  zosc.Init(sample_rate);
  zosc.SetFreq(80.f);
  zosc.SetShape(1.f);

  lfo1.Init(sample_rate);
  lfo1.SetAmp(1.f);
  lfo1.SetFreq(1.f);

  lfo2.Init(sample_rate);
  lfo2.SetAmp(1.f);
  lfo2.SetFreq(.5f);

  DAISY.begin(AudioCallback);
}
void loop() {}
