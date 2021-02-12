// Title: pitchshifter
// Description: Pitchshift input audio up an octave
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

PitchShifter ps;

void MyCallback(float **in, float **out, size_t size) {
  float shifted, unshifted;
  for (size_t i = 0; i < size; i++) {
    unshifted = in[0][i];
    shifted = ps.Process(unshifted);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = (chn % 2) == 0 ? shifted : unshifted;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  ps.Init(sample_rate);
  // set transposition 1 octave up (12 semitones)
  ps.SetTransposition(12.0f);

  DAISY.begin(MyCallback);
}

void loop() {}
