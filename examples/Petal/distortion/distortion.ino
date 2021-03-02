#include "DaisyDuino.h"

static DaisyHardware petal;

float hardClip(float in) {
  in = in > 1.f ? 1.f : in;
  in = in < -1.f ? -1.f : in;
  return in;
}

float softClip(float in) {
  if (in > 0)
    return 1 - expf(-in);
  return -1 + expf(in);
}

bool bypassHard, bypassSoft;
static void AudioCallback(float **in, float **out, size_t size) {
  petal.ProcessAllControls();

  float Pregain = petal.controls[2].Value() * 10 + 1.2;
  float Gain = petal.controls[3].Value() * 100 + 1.2;
  float drywet = petal.controls[4].Value();

  bypassSoft ^= petal.buttons[0].RisingEdge();
  bypassHard ^= petal.buttons[1].RisingEdge();

  for (size_t i = 0; i < size; i++) {
    for (int chn = 0; chn < 2; chn++) {
      in[chn][i] *= Pregain;
      float wet = in[chn][i];

      if (!bypassSoft || !bypassHard) {
        wet *= Gain;
      }

      if (!bypassSoft) {
        wet = softClip(wet);
      }

      if (!bypassHard) {
        wet = hardClip(wet);
      }

      out[chn][i] = wet * drywet + in[chn][i] * (1 - drywet);
    }
  }
}

void setup() {
  petal = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);

  bypassHard = bypassSoft = false;

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {
  // LED stuff
  petal.SetFootswitchLed(0, !bypassSoft);
  petal.SetFootswitchLed(1, !bypassHard);

  for (int i = 0; i < 8; i++) {
    petal.SetRingLed(i, 1.f, 0.f, 0.f);
  }

  petal.UpdateLeds();

  delay(6);
}