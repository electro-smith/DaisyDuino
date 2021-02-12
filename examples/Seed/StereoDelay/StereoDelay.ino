// Title: StereoDelay
// Description: Applies stereo delay to input signal
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

// two DelayLine of 24000 floats.
DelayLine<float, 24000> del_left, del_right;

void MyCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float dry_left, dry_right, wet_left, wet_right;

    // Read Dry from I/O
    dry_left = in[0][i];
    dry_right = in[1][i];

    // Read Wet from Delay Lines
    wet_left = del_left.Read();
    wet_right = del_right.Read();

    // Write to Delay with some feedback
    del_left.Write((wet_left * 0.5) + dry_left);
    del_right.Write((wet_right * 0.5) + dry_right);

    // Mix Dry and Wet and send to I/O
    out[0][i] = wet_left * 0.707 + dry_left * 0.707;
    out[1][i] = wet_right * 0.707 + dry_right * 0.707;
  }
}

void setup() {
  float samplerate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;

  // Init Delay Lines
  del_left.Init();
  del_right.Init();

  // Set Delay Times in Samples
  del_left.SetDelay(12000.0f);
  del_right.SetDelay(8000.0f);

  // Start Audio
  DAISY.begin(MyCallback);
}

void loop() {}
