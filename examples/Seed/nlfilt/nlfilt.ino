// Title: nlfilt
// Description: Random osc notes through nonlinear filter
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

// Helper Modules
static AdEnv env;
static Oscillator osc;
static Metro tick;

static NlFilt filt;

void MyCallback(float **in, float **out, size_t size) {
  // The NlFilt object currently only works on blocks of audio at a time.
  // This can be accomodated easily with an extra loop at the end.

  float dry[size];
  float wet[size];
  float env_out;
  // loop through mono process
  for (size_t i = 0; i < size; i++) {
    // When the Metro ticks:
    // trigger the envelope to start, and change freq of oscillator.
    if (tick.Process()) {
      float freq = rand() % 150;
      osc.SetFreq(freq + 25.0f);
      env.Trigger();
    }
    // Use envelope to control the amplitude of the oscillator.
    env_out = env.Process();
    osc.SetAmp(env_out);
    dry[i] = osc.Process();
  }

  // nonlinear filter
  filt.ProcessBlock(dry, wet, size);

  for (size_t i = 0; i < size; i++) {
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = wet[i];
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  env.Init(sample_rate);
  osc.Init(sample_rate);

  // Set up Metro to pulse every 3 seconds
  tick.Init(0.333f, sample_rate);

  // Set adenv parameters
  env.SetTime(ADENV_SEG_ATTACK, 1.50);
  env.SetTime(ADENV_SEG_DECAY, 1.50);
  env.SetMin(0.0);
  env.SetMax(0.25);
  env.SetCurve(0); // linear

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_POLYBLEP_SAW);

  // Set coefficients for non-linear filter.
  filt.SetCoefficients(0.7f, -0.2f, 0.95f, 0.24f, 1000.0f);

  DAISY.begin(MyCallback);
}

void loop() {}
