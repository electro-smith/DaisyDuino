// Title: port
// Description: Slides between random notes
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Port slew;
static Metro clock;
static Oscillator osc_sine;

float freq;

void MyCallback(float **in, float **out, size_t size) {
  float sine, slewed_freq;
  uint8_t tic;
  for (size_t i = 0; i < size; i++) {

    tic = clock.Process();
    if (tic) {
      freq = rand() % 500;
    }

    slewed_freq = slew.Process(freq);
    osc_sine.SetFreq(slewed_freq);

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

  // set params for Port object
  slew.Init(sample_rate, .09);

  clock.Init(1, sample_rate);

  // set parameters for sine oscillator object
  osc_sine.Init(sample_rate);
  osc_sine.SetWaveform(Oscillator::WAVE_SIN);
  osc_sine.SetFreq(100);
  osc_sine.SetAmp(0.25);

  DAISY.begin(MyCallback);
}

void loop() {}
