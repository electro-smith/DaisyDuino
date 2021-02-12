// Title: PatchSine
// Description: Generates a sine wave on all 4 outputs
// Hardware: Daisy Patch
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;
float phs_inc, phs, sinout;

void SineCallback(float **in, float **out, size_t size) {
  phs_inc = (2.0 * PI * 100.0) * (1.0 / 48000.0);
  for (size_t i = 0; i < size; i++) {
    // Synthesize sine wave.
    sinout = sin(phs) * 0.5;
    // update phase
    phs = phs + phs_inc;
    if (phs > 2.0 * PI)
      phs -= (2.0 * PI);

    // Set to outputs.
    out[0][i] = sinout;
    out[1][i] = sinout;
    out[2][i] = sinout;
    out[3][i] = sinout;
  }
}

void setup() {
  hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  DAISY.begin(SineCallback);
}

void loop() {}
