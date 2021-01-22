// Title: SequentialSwitch
// Description: Switch between inputs on triggers
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/SequentialSwitch/resources/SequentialSwitch.png
// Controls
//
// Gate In 1: Next In/Out
// Gate In 2: Reset to In/Out 1
//
// Encoder Press: Switch Modes
// Single In -> Multi Out:
// Audio In 1 -> Out 1-4
//
// Multi In -> Single Out:
// Audio In 1-4 -> Out 1

#include "DaisyDuino.h"
#include <U8g2lib.h>

DaisyHardware patch;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

bool isMultiIn = false;
uint8_t step = 0;

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    for (size_t chn = 0; chn < 4; chn++) {
      out[0][i] = out[1][i] = out[2][i] = out[3][i] =
          0.f; // reset non active outs

      if (isMultiIn)
        out[0][i] = in[step][i];
      else
        out[step][i] = in[0][i];
    }
  }
}

void setup() {
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();
  UpdateOled();

  DAISY.begin(AudioCallback);
}

void loop() { UpdateControls(); }

void UpdateControls() {
  patch.DebounceControls();

  if (patch.gateIns[0].Trig()) {
    step++;
    step %= 4;
  }

  if (patch.gateIns[1].Trig()) {
    step = 0;
  }

  if (patch.encoder.RisingEdge()) {
    isMultiIn = !isMultiIn;
    UpdateOled();
  }
}

void UpdateOled() {
  oled.clear();

  oled.drawString(0, 0, "Sequential");
  oled.drawString(0, 1, "Switch");

  String str = isMultiIn ? "Multi " : "Single ";
  str += "In -> ";
  char *cstr = &str[0];
  oled.drawString(0, 3, cstr);

  str = isMultiIn ? "Single " : "Multi ";
  str += "Out";
  oled.drawString(0, 4, cstr);
}
