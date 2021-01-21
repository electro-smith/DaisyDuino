
#include "DaisyDuino.h"

DaisyHardware hw;
Overdrive drive;
Oscillator osc, lfo;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    drive.SetDrive(fabsf(lfo.Process()));
    float sig = drive.Process(osc.Process());
    out[0][i] = out[1][i] = sig;
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);
  lfo.Init(sample_rate);
  lfo.SetAmp(.8f);
  lfo.SetWaveform(Oscillator::WAVE_TRI);
  lfo.SetFreq(.25f);

  DAISY.begin(AudioCallback);
}

void loop() {}
