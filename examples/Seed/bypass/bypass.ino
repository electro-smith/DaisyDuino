// Title: bypass
// Description: Passes left and right in to left and right out
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

void MyCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = in[chn][i];
    }
  }
}

void setup() {
  float samplerate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();

  DAISY.begin(MyCallback);
}

void loop() {}
