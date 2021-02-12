// Title: allpass
// Description: Allpass filter frequency is swept with sine wave oscillator as
// input Hardware: Daisy Seed Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware seed;

static Oscillator osc, lfo;
static Allpass allpass;

static void AudioCallback(float **in, float **out, size_t size) {
  float sig;
  for (size_t i = 0; i < size; i++) {
    sig = osc.Process();
    float l = .01 + lfo.Process();
    allpass.SetFreq(l);
    sig = allpass.Process(sig);

    out[0][i] = out[1][i] = sig;
  }
}

void setup() {
  // initialize seed hardware and oscillator daisysp module
  float sample_rate;
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);
  lfo.Init(sample_rate);

  float buff[9600];
  for (int i = 0; i < 9600; i++) {
    buff[i] = 0.0f;
  }

  allpass.Init(sample_rate, buff, (int)9600);

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_SIN);
  osc.SetFreq(440);
  osc.SetAmp(0.5);

  // Set parameters for oscillator
  lfo.SetWaveform(osc.WAVE_SIN);
  lfo.SetFreq(1);
  lfo.SetAmp(0.01);

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {}
