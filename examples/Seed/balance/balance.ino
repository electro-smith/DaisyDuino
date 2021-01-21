// Title: balance
// Description: Sets one signal to the level of another
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc1, osc2, lfo;
static Balance bal;

void MyCallback(float **in, float **out, size_t size) {
  float sig1, sig2, sig3;
  for (size_t i = 0; i < size; i++) {
    sig1 = osc1.Process();
    sig2 = osc2.Process();
    sig3 = lfo.Process();
    sig2 *= sig3;
    sig1 = bal.Process(sig1, sig2);
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig1;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  osc1.Init(sample_rate);
  osc2.Init(sample_rate);
  lfo.Init(sample_rate);
  bal.Init(sample_rate);

  // Set parameters for oscillator one
  osc1.SetWaveform(osc1.WAVE_SIN);
  osc1.SetFreq(440);
  osc1.SetAmp(0.1);

  // Set parameters for oscillator two
  osc2.SetWaveform(osc2.WAVE_SIN);
  osc2.SetFreq(220);
  osc2.SetAmp(0.5);

  // Set parameters for lfo
  lfo.SetWaveform(lfo.WAVE_TRI);
  lfo.SetFreq(0.4);
  lfo.SetAmp(0.5);

  DAISY.begin(MyCallback);
}

void loop() {}
