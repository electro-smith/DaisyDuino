
#include "DaisyDuino.h"

DaisyHardware hw;
VosimOscillator vosim;
Oscillator lfo;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float mod = lfo.Process();
    vosim.SetForm2Freq(817.2f * (mod + 1.f));
    vosim.SetShape(mod);

    out[0][i] = out[1][i] = vosim.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  vosim.Init(sample_rate);
  vosim.SetFreq(105.f);
  vosim.SetForm1Freq(1390.7);

  lfo.Init(sample_rate);
  lfo.SetAmp(1.f);
  lfo.SetFreq(.5f);

  DAISY.begin(AudioCallback);
}

void loop() {}
