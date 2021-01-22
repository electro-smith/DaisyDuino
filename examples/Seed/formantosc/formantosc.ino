#include "DaisyDuino.h"

static DaisyHardware seed;
static FormantOscillator form[2];
static Metro tick;
static Oscillator lfo;

float freqs[3][2] = {{390, 2300}, {610, 1900}, {820, 1530}};
int idx = 0;

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (tick.Process()) {
      idx = (idx + 1) % 3;
    }

    form[0].SetPhaseShift(lfo.Process());
    form[1].SetPhaseShift(lfo.Process());

    form[0].SetFormantFreq(freqs[idx][0]);
    form[1].SetFormantFreq(freqs[idx][1]);
    out[0][i] = out[1][i] = (form[0].Process() * .6f + form[1].Process() * .4f);
  }
}

void setup() {
  // initialize seed hardware and oscillator daisysp module
  float sample_rate;
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  sample_rate = DAISY.get_samplerate();

  form[0].Init(sample_rate);
  form[0].SetCarrierFreq(200.f);

  form[1].Init(sample_rate);
  form[1].SetCarrierFreq(200.f);

  lfo.Init(sample_rate);
  lfo.SetAmp(1.f);
  lfo.SetFreq(.2f);

  tick.Init(1.f, sample_rate);

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {}
