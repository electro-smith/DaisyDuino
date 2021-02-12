// Title: whitenoise
// Description: Generates whitenoise
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static WhiteNoise nse;

void MyCallback(float **in, float **out, size_t size) {
  float sig;
  for (size_t i = 0; i < size; i++) {
    sig = nse.Process();

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

  nse.Init();

  DAISY.begin(MyCallback);
}

void loop() {}
