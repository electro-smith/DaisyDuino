// Title: biquad
// Description: Biquadratic resonant filter.
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Biquad flt;
static Oscillator osc, lfo;

void MyCallback(float **in, float **out, size_t size) {
  float saw, freq, output;
  for (size_t i = 0; i < size; i++) {
    freq = 10000 + (lfo.Process() * 10000);
    saw = osc.Process();

    flt.SetCutoff(freq);
    output = flt.Process(saw);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = output;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  // initialize Biquad and set parameters
  flt.Init(sample_rate);
  flt.SetRes(0.7);

  // set parameters for sine oscillator object
  lfo.Init(sample_rate);
  lfo.SetWaveform(Oscillator::WAVE_TRI);
  lfo.SetAmp(1);
  lfo.SetFreq(.4);

  // set parameters for sine oscillator object
  osc.Init(sample_rate);
  osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
  osc.SetFreq(100);
  osc.SetAmp(0.25);

  DAISY.begin(MyCallback);
}

void loop() {}
