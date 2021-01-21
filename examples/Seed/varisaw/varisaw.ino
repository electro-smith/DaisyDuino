
#include "DaisyDuino.h"

DaisyHardware hw;
VariableSawOscillator varisaw;
Oscillator lfo;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float sig = lfo.Process();
    varisaw.SetPW(sig);

    out[0][i] = out[1][i] = varisaw.Process();
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  varisaw.Init(sample_rate);
  varisaw.SetFreq(62.5f);
  varisaw.SetWaveshape(1.f);

  lfo.Init(sample_rate);
  lfo.SetAmp(1.f);
  lfo.SetFreq(1.f);

  DAISY.begin(AudioCallback);
}
void loop() {}
