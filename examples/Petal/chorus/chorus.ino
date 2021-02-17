
#include "DaisyDuino.h"

DaisyHardware hw;
Chorus ch;

bool effectOn;
float wet;

float deltarget, del;
float lfotarget, lfo;

void Controls() {
  hw.ProcessAllControls();

  // knobs
  float k = hw.controls[2].Process();
  ch.SetLfoFreq(k * k * 20.f);
  lfo = hw.controls[3].Process();
  del = hw.controls[4].Process();
  ch.SetFeedback(hw.controls[5].Process());

  // footswitch
  effectOn ^= hw.buttons[0].RisingEdge();

  // encoder
  wet += hw.encoder.Increment() * .05f;
  wet = fclamp(wet, 0.f, 1.f);

  wet = hw.encoder.RisingEdge() ? .9f : wet;
}

void AudioCallback(float **in, float **out, size_t size) {
  Controls();

  for (size_t i = 0; i < size; i++) {
    fonepole(del, deltarget, .0001f); // smooth at audio rate
    ch.SetDelay(del);

    fonepole(lfo, lfotarget, .0001f); // smooth at audio rate
    ch.SetLfoDepth(lfo);

    out[0][i] = in[0][i];
    out[1][i] = in[1][i];

    if (effectOn) {
      ch.Process(in[0][i]);

      out[0][i] = ch.GetLeft() * wet + in[0][i] * (1.f - wet);
      out[1][i] = ch.GetRight() * wet + in[1][i] * (1.f - wet);
    }
  }
}

void setup() {
  hw = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);
  float sample_rate = DAISY.AudioSampleRate();

  ch.Init(sample_rate);

  effectOn = true;
  wet = .9f;
  deltarget = del = 0.f;
  lfotarget = lfo = 0.f;

  DAISY.begin(AudioCallback);
}
void loop() {
  delay(6);

  hw.ClearLeds();
  hw.SetFootswitchLed(0, (float)effectOn);

  int wet_int = (int)(wet * 8.f);
  float wet_frac = wet - wet_int;
  for (int i = 0; i < wet_int; i++) {
    hw.SetRingLed(i, 1.f, 0.f, 0.f);
  }

  if (wet_int < 8) {
    hw.SetRingLed(wet_int, wet_frac, 0.f, 0.f);
  }

  hw.UpdateLeds();
}
}
