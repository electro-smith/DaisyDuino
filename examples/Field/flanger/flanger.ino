
#include "DaisyDuino.h"

// the magic incantation
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI
    oled(U8G2_R0, /* clock=*/8, /* data=*/10, /* cs=*/7, /* dc=*/9);

Flanger flanger;
DaisyHardware hw;

bool effectOn = true;

float del, deltarget;
float lfo, lfotarget;

float knob_vals[4];
void AudioCallback(float **in, float **out, size_t size) {
  hw.ProcessAllControls();

  for (int i = 0; i < 4; i++) {
    knob_vals[i] = hw.knob[i].Process();
  }

  deltarget = hw.knob[0].Process();
  flanger.SetFeedback(hw.knob[1].Process());
  float val = hw.knob[2].Process();
  flanger.SetLfoFreq(val * val * 10.f);
  lfotarget = hw.knob[3].Process();

  effectOn ^= hw.sw[0].RisingEdge();

  for (size_t i = 0; i < size; i++) {
    fonepole(del, deltarget, .0001f); // smooth at audio rate
    flanger.SetDelay(del);

    fonepole(lfo, lfotarget, .0001f); // smooth at audio rate
    flanger.SetLfoDepth(lfo);

    out[0][i] = out[1][i] = effectOn ? flanger.Process(in[0][i]) : in[0][i];
  }
}

void setup() {
  hw = DAISY.init(DAISY_FIELD, AUDIO_SR_48K);
  float sample_rate = DAISY.AudioSampleRate();

  flanger.Init(sample_rate);

  del = deltarget = 0.f;
  lfo = lfotarget = 0.f;

  DAISY.begin(AudioCallback);
}
void loop() {
  oled.clearBuffer();

  char cstr[11];
  sprintf(cstr, "Effect %s", effectOn ? "On" : "Off");
  oled.drawStr(0, 0, cstr);

  oled.sendBuffer()();

  size_t knob_leds[] = {
      DaisyHardware::LED_KNOB_1,
      DaisyHardware::LED_KNOB_2,
      DaisyHardware::LED_KNOB_3,
      DaisyHardware::LED_KNOB_4,
  };

  for (size_t i = 0; i < 4; i++) {
    hw.led_driver.SetLed(knob_leds[i], knob_vals[i]);
  }

  hw.led_driver.SwapBuffersAndTransmit();
}
