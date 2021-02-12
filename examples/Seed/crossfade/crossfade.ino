// Title: crossfade
// Description: Fades between a saw and a sine oscillator
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static CrossFade cfade;
static Oscillator osc_sine, osc_saw, lfo;

void MyCallback(float **in, float **out, size_t size) {
  float saw, sine, pos, output;
  for (size_t i = 0; i < size; i++) {
    sine = osc_sine.Process();
    saw = osc_saw.Process();

    // lfo output = -1 to 1
    pos = lfo.Process();

    // scale signal between 0 and 1
    pos = (pos + 1) / 2;

    cfade.SetPos(pos);
    output = cfade.Process(sine, saw);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = output;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  // set params for CrossFade object
  cfade.Init();
  cfade.SetCurve(CROSSFADE_LIN);

  // set parameters for sine oscillator object
  osc_sine.Init(sample_rate);
  osc_sine.SetWaveform(Oscillator::WAVE_SIN);
  osc_sine.SetFreq(100);
  osc_sine.SetAmp(0.25);

  // set parameters for sawtooth oscillator object
  osc_saw.Init(sample_rate);
  osc_saw.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
  osc_saw.SetFreq(100);
  osc_saw.SetAmp(0.25);

  // set parameters for triangle lfo oscillator object
  lfo.Init(sample_rate);
  lfo.SetWaveform(Oscillator::WAVE_TRI);
  lfo.SetFreq(.25);
  lfo.SetAmp(1);

  DAISY.begin(MyCallback);
}

void loop() {}
