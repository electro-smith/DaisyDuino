
#include "DaisyDuino.h"
#include <stdlib.h>

DaisyHardware hw;
ModalVoice modal;
Metro tick;
Oscillator lfo;

// A minor pentatonic
float freqs[5] = {440.f, 523.25f, 587.33f, 659.25f, 783.99f};
bool sus = false;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    bool t = tick.Process();
    if (t) {
      modal.SetSustain(sus = !sus);
    }

    if (t || modal.GetAux() > .1f) {
      modal.SetFreq(freqs[rand() % 5]);
    }

    float sig = fabsf(lfo.Process());
    modal.SetStructure(sig);
    modal.SetBrightness(.1f + sig * .1f);

    out[0][i] = out[1][i] = modal.Process(t);
  }
}

void setup() {
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  float sample_rate = DAISY.get_samplerate();

  modal.Init(sample_rate);
  modal.SetDamping(.5);

  tick.Init(2.f, sample_rate);

  lfo.Init(sample_rate);
  lfo.SetFreq(.005f);
  lfo.SetAmp(1.f);

  DAISY.begin(AudioCallback);
}

void loop() {}
