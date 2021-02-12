// Title: decimator
// Description: Generates a pure sine wave, and then crushes 8 bits, and
// variably downsamples the tone. Hardware: Daisy Seed Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc;
static Decimator decim;
static Phasor phs;

void MyCallback(float **in, float **out, size_t size) {
  float osc_out, decimated_out;
  float downsample_amt;
  for (size_t i = 0; i < size; i++) {
    // Generate a pure sine wave
    osc_out = osc.Process();
    // Modulate downsample amount via Phasor
    downsample_amt = phs.Process();
    decim.SetDownsampleFactor(downsample_amt);
    // downsample and bitcrush
    decimated_out = decim.Process(osc_out);
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = decimated_out;
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
  phs.Init(sample_rate, 0.5f);
  decim.Init();

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_SIN);
  osc.SetFreq(220);
  osc.SetAmp(0.25);
  // Set downsampling, and bit crushing values.
  decim.SetDownsampleFactor(0.4f);
  decim.SetBitsToCrush(8);

  DAISY.begin(MyCallback);
}

void loop() {}
