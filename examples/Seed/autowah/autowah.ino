// Title: autowah
// Description: Wahs automatically
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static AdEnv env;
static Oscillator osc;
static Metro tick;
static Autowah autowah;

void MyCallback(float **in, float **out, size_t size) {
  float osc_out, env_out;
  for (size_t i = 0; i < size; i++) {

    // When the metro ticks, trigger the envelope to start.
    if (tick.Process()) {
      env.Trigger();
    }

    // Use envelope to control the amplitude of the oscillator.
    // Apply autowah on the signal.
    env_out = env.Process();
    osc.SetAmp(env_out);
    osc_out = osc.Process();
    osc_out = autowah.Process(osc_out);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = osc_out;
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
  autowah.Init(sample_rate);

  // Set up metro to pulse every second
  tick.Init(1.0f, sample_rate);

  // set adenv parameters
  env.SetTime(ADENV_SEG_ATTACK, 0.01);
  env.SetTime(ADENV_SEG_DECAY, 0.5);
  env.SetMin(0.0);
  env.SetMax(0.5);
  env.SetCurve(0); // linear

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_SAW);
  osc.SetFreq(220);
  osc.SetAmp(0.25);

  // set autowah parameters
  autowah.SetLevel(.1);
  autowah.SetDryWet(100);
  autowah.SetWah(1);

  DAISY.begin(MyCallback);
}

void loop() {}
