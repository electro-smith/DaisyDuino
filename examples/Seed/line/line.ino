// Title: line
// Description: Generates a straight line between 2 points, then uses that to
// set an osc pitch Hardware: Daisy Seed Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Line line_seg;
static Oscillator osc_sine;

uint8_t finished;

void MyCallback(float **in, float **out, size_t size) {
  float sine, freq;
  for (size_t i = 0; i < size; i++) {

    if (finished) {
      // Start creating a Line segment from 100 to 500 in 1 seconds
      line_seg.Start(100, 500, 1);
    }

    freq = line_seg.Process(&finished);
    osc_sine.SetFreq(freq);
    sine = osc_sine.Process();

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sine;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  // initialize Line module
  line_seg.Init(sample_rate);
  finished = 1;

  // set parameters for sine oscillator object
  osc_sine.Init(sample_rate);
  osc_sine.SetWaveform(Oscillator::WAVE_SIN);
  osc_sine.SetFreq(100);
  osc_sine.SetAmp(0.25);

  DAISY.begin(MyCallback);
}

void loop() {}
