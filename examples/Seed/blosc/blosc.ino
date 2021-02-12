// Title: blosc
// Description: Band limited oscillator. Avoids aliasing issues.
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static BlOsc osc;
static Metro tick;

int waveform = 0;

void MyCallback(float **in, float **out, size_t size) {
  float sig;
  for (size_t i = 0; i < size; i++) {
    // switch waveforms
    if (tick.Process()) {
      waveform++;
      osc.SetWaveform(waveform % 3);
    }

    sig = osc.Process();

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);

  // Set up metro to pulse every second
  tick.Init(1.0f, sample_rate);

  // Set parameters for oscillator;
  osc.SetFreq(440);
  osc.SetAmp(0.5);
  osc.SetPw(.5);

  DAISY.begin(MyCallback);
}

void loop() {}
