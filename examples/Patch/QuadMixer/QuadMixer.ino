// Input  1-4 = channel 1-4 in
// Ctrl   1-4 = channel 1-4 level
// Output 1-4 = Mixer output

#include "DaisyDuino.h"
#include <U8g2lib.h>

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);

DaisyHardware patch;
float ctrlVal[4];
static void AudioCallback(float **in, float **out, size_t size) {
  int pins[4] = {PIN_PATCH_CTRL_1, PIN_PATCH_CTRL_2, PIN_PATCH_CTRL_3, PIN_PATCH_CTRL_4};

  for (int i = 0; i < 4; i++) {
    // Get the four control values
    ctrlVal[i] = (float)(1023 - analogRead(pins[i])) / 1023.f;
  }

  for (size_t i = 0; i < size; i++) {
    float output = 0.f;
    // sum all four inputs, attenuated by the control levels
    for (size_t chn = 0; chn < 4; chn++) {
      output += ctrlVal[chn] * in[chn][i];
    }

    // attenuate by 1/4
    output *= .25f;

    // send the same thing to all 4 outputs
    for (size_t chn = 0; chn < 4; chn++) {
      out[chn][i] = output;
    }
  }
}

void setup(){
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);

  oled.setFont(u8g2_font_t0_12_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();

  // start audio
  DAISY.begin(AudioCallback);
}

void loop(){
  // display the four control levels
  oled.clearBuffer();

  char cstr[15];
  for(int i = 0; i < 4; i++){
    sprintf(cstr, "channel %d: %d", i + 1, (int)(ctrlVal[i] * 101.f));
    oled.drawStr(0, (i + 1) * 10, cstr);
  }
  
  oled.sendBuffer();
}
