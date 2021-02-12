// Title: reverbsc
// Description: Applies reverb to input signal
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

ReverbSc verb;

void MyCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float out1, out2;
    verb.Process(in[0][i], in[1][i], &out1, &out2);

    out[0][i] = out1;
    out[1][i] = out2;
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  verb.Init(sample_rate);
  verb.SetFeedback(0.85f);
  verb.SetLpFreq(18000.0f);

  DAISY.begin(MyCallback);
}

void loop() {}
