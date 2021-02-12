// Title: svf
// Description: Lowpass out left, highpass out right
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

Oscillator osc;
Svf filt;

void MyCallback(float **in, float **out, size_t size) {
  float sig;
  for (size_t i = 0; i < size; i++) {
    sig = osc.Process();
    filt.Process(sig);

    out[0][i] = filt.Low();
    out[1][i] = filt.High();
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  // Initialize Oscillator, and set parameters.
  osc.Init(sample_rate);
  osc.SetWaveform(osc.WAVE_POLYBLEP_SAW);
  osc.SetFreq(250.0);
  osc.SetAmp(0.5);
  // Initialize Filter, and set parameters.
  filt.Init(sample_rate);
  filt.SetFreq(500.0);
  filt.SetRes(0.85);
  filt.SetDrive(0.8);

  DAISY.begin(MyCallback);
}

void loop() {}
