// Title: dcblock
// Description: Removes offset from signals
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static DcBlock block;
static Oscillator osc_sine;

void MyCallback(float **in, float **out, size_t size) {
  float output;
  for (size_t i = 0; i < size; i++) {

    output = osc_sine.Process();

    // add dc to signal
    output += 1;

    // remove dc from signal
    output = block.Process(output);

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

  block.Init(sample_rate);

  // set parameters for sine oscillator object
  osc_sine.Init(sample_rate);
  osc_sine.SetWaveform(Oscillator::WAVE_SIN);
  osc_sine.SetFreq(100);
  osc_sine.SetAmp(0.25);

  DAISY.begin(MyCallback);
}

void loop() {}
