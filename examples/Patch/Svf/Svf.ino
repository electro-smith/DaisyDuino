// Title: Svf
// Description: State Variable Filter. Single filter with multiple output types
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/Svf/resources/Svf.png
// Controls
// Ctrl 1: Filter Cutoff
// Ctrl 2: Resonance
// Ctrl 3: Drive

#include "DaisyDuino.h"
#include <U8g2lib.h>

DaisyHardware patch;
Svf svf;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

float GetCtrl(uint8_t pin) { return (1023.f - analogRead(pin)) / 1023.f; }

// Floating point implementation of Arduino map function
static float fmap(float x, float in_min, float in_max, float out_min,
                  float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // send the next sample to the filter
    svf.Process(in[0][i]);

    // send the different filter types to the different outputs
    out[0][i] = svf.Low();
    out[1][i] = svf.High();
    out[2][i] = svf.Band();
    out[3][i] = svf.Notch();
  }
}

void setup() {
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  // Initialize filter
  svf.Init(samplerate);

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();

  // Put controls onscreen
  oled.drawString(0, 0, "Cut  Res  Drive");
  oled.drawString(0, 6, "LP HP BP Notch");

  DAISY.begin(AudioCallback);
}

void loop() {
  // get new control values
  float cutoff = GetCtrl(PIN_PATCH_CTRL_1) * 20000.f;
  float res = fmap(GetCtrl(PIN_PATCH_CTRL_2), 0, 1, .3, .99);
  float drive = fmap(GetCtrl(PIN_PATCH_CTRL_3), 0, 1, .3, 1);

  // Set filter to the values we got
  svf.SetFreq(cutoff);
  svf.SetRes(res);
  svf.SetDrive(drive);
}
