// jitter module example

#include "DaisyDuino.h"

// Shortening long macro for sample rate
#ifndef sample_rate

#endif

static DaisyHardware seed;
static Jitter jitter;
static Oscillator osc;
bool gate;

static void AudioCallback(float **in, float **out, size_t size) {
  float osc_out, jitter_out;
  for (size_t i = 0; i < size; i++) {
    // Use jitter to control the amplitude of the oscillator.
    jitter_out = jitter.Process();
    osc.SetAmp(jitter_out);
    osc_out = osc.Process();

    out[0][i] = osc_out;
    out[1][i] = osc_out;
  }
}

void setup() {
  // initialize seed hardware and daisysp modules
  float sample_rate;
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  sample_rate = DAISY.get_samplerate();
  osc.Init(sample_rate);

  // set jitter parameters
  jitter.Init(sample_rate);
  jitter.SetAmp(1);
  jitter.SetCpsMin(1);
  jitter.SetCpsMax(25);

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_TRI);
  osc.SetFreq(440);
  osc.SetAmp(0.25);

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {}
