// Title: compressor
// Description:
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Compressor comp;
// Helper Modules
static AdEnv env;
static Oscillator osc_a, osc_b;
static Metro tick;

void MyCallback(float **in, float **out, size_t size) {
  float osc_a_out, osc_b_out, env_out, sig_out;
  for (size_t i = 0; i < size; i++) {

    // When the metro ticks:
    // trigger the envelope to start
    if (tick.Process()) {
      env.Trigger();
    }

    // Use envelope to control the amplitude of the oscillator.
    env_out = env.Process();
    osc_a.SetAmp(env_out);
    osc_a_out = osc_a.Process();
    osc_b_out = osc_b.Process();
    // Compress the steady tone with the enveloped tone.
    sig_out = comp.Process(osc_b_out, osc_a_out);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig_out;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  comp.Init(sample_rate);
  env.Init(sample_rate);
  osc_a.Init(sample_rate);
  osc_b.Init(sample_rate);

  // Set up metro to pulse every second
  tick.Init(1.0f, sample_rate);

  // set compressor parameters
  comp.SetThreshold(-64.0f);
  comp.SetRatio(2.0f);
  comp.SetAttack(0.005f);
  comp.SetRelease(0.1250);

  // set adenv parameters
  env.SetTime(ADENV_SEG_ATTACK, 0.001);
  env.SetTime(ADENV_SEG_DECAY, 0.50);
  env.SetMin(0.0);
  env.SetMax(0.25);
  env.SetCurve(0); // linear

  // Set parameters for oscillator
  osc_a.SetWaveform(Oscillator::WAVE_TRI);
  osc_a.SetFreq(110);
  osc_a.SetAmp(0.25);
  osc_b.SetWaveform(Oscillator::WAVE_TRI);
  osc_b.SetFreq(220);
  osc_b.SetAmp(0.25);

  DAISY.begin(MyCallback);
}

void loop() {}
