// sample and hold / track and hold
// 2 circuits are available
// Gate In 1-2 = Circuit 1-2 Gate In
// Ctrl 1-2 = Circuit 1-2 Input
// CV Out 1-2 = Circuit 1-2 out
// Encoder turn / press to set circuit modes

#include "DaisyDuino.h"
#include <string>
#include <U8g2lib.h>

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);

DaisyHardware patch;

struct sampHoldStruct {
  SampleHold sampHold;
  SampleHold::Mode mode;
  float output;

  void Process(bool trigger, float input) {
    output = sampHold.Process(trigger, input, mode);
  }
};

sampHoldStruct sampHolds[2];

int menuPos;

//using this to get controls and outputs in a callback away from oled loop
void Callback(float **in, float **out, size_t size){
  //controls
  patch.DebounceControls();

  float ctrl1 = (1023 - analogRead(PIN_PATCH_CTRL_1)) / 1023.0;
  float ctrl2 = (1023 - analogRead(PIN_PATCH_CTRL_2)) / 1023.0;

  // read ctrls and gates, then update sampleholds
  sampHolds[0].Process(patch.gateIns[0].State(), ctrl1);
  sampHolds[1].Process(patch.gateIns[1].State(), ctrl2);

  // encoder
  menuPos += patch.encoder.Increment();
  menuPos = (menuPos % 2 + 2) % 2;

  // switch modes
  if (patch.encoder.RisingEdge()) {
    sampHolds[menuPos].mode =
        (SampleHold::Mode)((sampHolds[menuPos].mode + 1) % 2);
  }

  //write to outputs
  analogWrite(PIN_PATCH_CV_1, sampHolds[0].output * 255.f);
  analogWrite(PIN_PATCH_CV_2, sampHolds[1].output * 255.f);
}


void setup(){
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);

  oled.setFont(u8g2_font_t0_12_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();

  
  DAISY.begin(Callback);
}

void loop(){
  oled.clearBuffer();

  std::string str = "Sample/Track and Hold";
  char *cstr = &str[0];
  oled.drawStr(0, 8, cstr);

  // Cursor
  str = "o";
  oled.drawStr(menuPos * 60, 33, cstr);

  // two circuits
  str = sampHolds[0].mode == 0 ? "S&H" : "T&H";
  oled.drawStr(0, 43, cstr);
  
  str = sampHolds[1].mode == 0 ? "S&H" : "T&H";
  oled.drawStr(60, 43, cstr);

  oled.sendBuffer();
}