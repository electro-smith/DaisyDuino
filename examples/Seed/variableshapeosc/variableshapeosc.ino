
#include "DaisyDuino.h"

DaisyHardware hw;
VariableShapeOscillator variosc;
Oscillator lfo, lfo2;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float mod = lfo.Process();
    float mod2 = lfo2.Process();
    variosc.SetSyncFreq(110.f * (mod + 3));
    variosc.SetPW(mod * .8f);
    variosc.SetWaveshape(mod2);

    out[0][i] = out[1][i] = variosc.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  variosc.Init(sample_rate);
  variosc.SetSync(true);
  variosc.SetFreq(110.f);

  lfo.Init(sample_rate);
  lfo.SetAmp(1.f);
  lfo.SetFreq(.25f);

  lfo2.Init(sample_rate);
  lfo2.SetAmp(1.f);
  lfo2.SetFreq(.125f);

  DAISY.begin(AudioCallback);
}

void loop() {}
