// Title: Mixer
// Description: Four Channel Mixer
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/QuadMixer/resources/QuadMixer.png
//
// Controls
// Ctrls: Channel 1-4 Amplitude
// Audio Ins: Channel 1-4 Input
// Audio Outs: Mix Output

#include "DaisyDuino.h"
#include <U8g2lib.h>
#include <U8x8lib.h>

#define MAX_WAVE Oscillator::WAVE_POLYBLEP_TRI

DaisyHardware patch;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

float amp[4];

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float sig = 0.f;
    for (size_t chn = 0; chn < 4; chn++) {
      sig += amp[chn] * in[chn][i];
    }

    sig *= .25;
    out[0][i] = out[1][i] = out[2][i] = out[3][i] = sig;
  }
}

void setup() {
  // put your setup code here, to run once:
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  amp[0] = amp[1] = amp[2] = amp[3] = 0.f;

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();
  oled.drawString(0, 0, "Mixer");

  DAISY.begin(AudioCallback);
}

void loop() {
  amp[0] = (1023 - analogRead(PIN_PATCH_CTRL_1)) / 1023.f;
  amp[1] = (1023 - analogRead(PIN_PATCH_CTRL_2)) / 1023.f;
  amp[2] = (1023 - analogRead(PIN_PATCH_CTRL_3)) / 1023.f;
  amp[3] = (1023 - analogRead(PIN_PATCH_CTRL_4)) / 1023.f;
}
