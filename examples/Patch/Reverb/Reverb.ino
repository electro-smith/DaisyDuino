// Title: Reverb
// Description: Stereo Reverb
// Hardware: Daisy Patch
// Author: Stephen Hensley
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/verb/resources/verb.png
// Controls
// Ctrl 1: Dry/Wet
// Ctrl 2: Send Amount
// Ctrl 3: Feedback Amount
// Ctrl 4: Lp filter Cutoff / Color
// Audio In 1-2: L R Inputs
// Audio Out 1-2: Dry/wet L R outputs
// Audio Out 3-4: Wet L R outputs

#include "DaisyDuino.h"
#include <U8g2lib.h>
#include <U8x8lib.h>

DaisyHardware patch;

static ReverbSc verb;
static DcBlock blk[2];
static float drylevel, send;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

float CtrlVal(uint8_t pin) { return (1023.f - analogRead(pin)) / 1023.f; }

void AudioCallback(float **in, float **out, size_t size) {
  float dryL, dryR, wetL, wetR, sendL, sendR;
  for (size_t i = 0; i < size; i++) {
    // read some controls

    // Read Inputs (only stereo in are used)
    dryL = in[0][i];
    dryR = in[1][i];

    // Send Signal to Reverb
    sendL = dryL * send;
    sendR = dryR * send;
    verb.Process(sendL, sendR, &wetL, &wetR);

    // Dc Block
    wetL = blk[0].Process(wetL);
    wetR = blk[1].Process(wetR);

    // Out 1 and 2 are Mixed
    out[0][i] = (dryL * drylevel) + wetL;
    out[1][i] = (dryR * drylevel) + wetR;

    // Out 3 and 4 are just wet
    out[2][i] = wetL;
    out[3][i] = wetR;
  }
}

void setup() {
  // put your setup code here, to run once:
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  verb.Init(samplerate);
  verb.SetFeedback(0.85f);
  verb.SetLpFreq(18000.0f);

  blk[0].Init(samplerate);
  blk[1].Init(samplerate);

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();

  oled.drawString(0, 0, "Stereo Reverb");

  DAISY.begin(AudioCallback);
}

void loop() {
  drylevel = CtrlVal(PIN_PATCH_CTRL_1);
  send = CtrlVal(PIN_PATCH_CTRL_2);
  verb.SetFeedback(CtrlVal(PIN_PATCH_CTRL_3) * .94f);

  verb.SetLpFreq(CtrlVal(PIN_PATCH_CTRL_4) * 20000 + 20); // linear
}
