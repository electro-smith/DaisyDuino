// Title: maytrig
// Description: Probabilistically triggers an adenv
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static AdEnv env;
static Oscillator osc;
static Maytrig maytrig;
static Metro tick;

void MyCallback(float **in, float **out, size_t size) {
  float osc_out, env_out;
  for (size_t i = 0; i < size; i++) {

    // When the metro ticks, trigger the envelope to start.
    if (tick.Process()) {
      if (maytrig.Process(.7)) {
        env.Trigger();
        osc.SetFreq(((float)rand() / RAND_MAX) * 800 + 200);
      }
    }

    // Use envelope to control the amplitude of the oscillator.
    env_out = env.Process();
    osc.SetAmp(env_out);
    osc_out = osc.Process();
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
  tick.Init(1, sample_rate);

  // set adenv parameters
  env.SetTime(ADENV_SEG_ATTACK, 0.01);
  env.SetTime(ADENV_SEG_DECAY, 0.1);
  env.SetMin(0.0);
  env.SetMax(0.25);
  env.SetCurve(0.5);

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_TRI);
  osc.SetFreq(220);
  osc.SetAmp(0.25);

  DAISY.begin(MyCallback);
}

void loop() {}
