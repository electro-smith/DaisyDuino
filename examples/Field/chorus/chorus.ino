
#include "DaisyDuino.h"

// the magic incantation
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI
    oled(U8G2_R0, /* clock=*/8, /* data=*/10, /* cs=*/7, /* dc=*/9);

DaisyHardware hw;
Chorus ch;

bool effectOn;

float deltarget0, del0;
float deltarget1, del1;
float lfotarget, lfo;

void Controls() {
  hw.ProcessAllControls();

  // knobs
  float k = hw.knob[0].Process() * .5f;
  ch.SetPan(.5f - k, .5f + k);

  k = hw.knob[1].Process();
  ch.SetLfoFreq(k * k * 20.f);

  lfotarget = hw.knob[2].Process();
  deltarget0 = hw.knob[3].Process();
  deltarget1 = hw.knob[4].Process();

  ch.SetFeedback(hw.knob[5].Process());

  // activate / deactivate effect
  effectOn ^= hw.sw[0].RisingEdge();
}

void AudioCallback(float **in, float **out, size_t size) {
  Controls();

  for (size_t i = 0; i < size; i++) {
    fonepole(del0, deltarget0, .0001f); // smooth these at audio rate
    fonepole(del1, deltarget1, .0001f);
    fonepole(lfo, lfotarget, .0001f);

    ch.SetLfoDepth(lfo);
    ch.SetDelay(del0, del1);

    ch.Process(in[0][i]);
    out[0][i] = effectOn ? ch.GetLeft() : in[0][i];
    out[1][i] = effectOn ? ch.GetRight() : in[0][i];
  }
}

void setup() {
  hw = DAISY.init(DAISY_FIELD, AUDIO_SR_48K);
  float sample_rate = DAISY.AudioSampleRate();

  ch.Init(sample_rate);

  deltarget0 = del0 = 0.f;
  deltarget1 = del1 = 0.f;
  lfotarget = lfo = 0.f;

  effectOn = true;

  DAISY.begin(AudioCallback);
}
void loop() {
  oled.clearBuffer();

  char cstr[15];
  sprintf(cstr, "Effect: %s", effectOn ? "On" : "Off");
  oled.drawStr(0, 0, cstr);

  oled.sendBuffer()();
}
