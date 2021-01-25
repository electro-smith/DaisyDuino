// Gate In 1-2: Trigger either (or both) to advance sequencer.
// Gate Out: Outputs triggers on x'd steps (second menu row)
// CV Out: Outputs current step CV. (first menu row).
// Encoder: Use to navigate menu, set levels and triggers.

#include "DaisyDuino.h"
#include <string>
#include <U8g2lib.h>

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);

DaisyHardware patch;

int values[5];
bool trigs[5];
int stepNumber;
bool trigOut;

int menuPos;
bool inSubMenu;

//just used to get controls and outputs in callback apart from OLED code
void Callback(float **in, float **out, size_t size){
  //controls
  patch.DebounceControls();

  // encoder
  if (!inSubMenu) {
    menuPos += patch.encoder.Increment();
    menuPos = (menuPos % 10 + 10) % 10;

    if (menuPos < 5) {
      inSubMenu = patch.encoder.RisingEdge() ? true : false;
    } else {
      trigs[menuPos % 5] =
          patch.encoder.RisingEdge() ? !trigs[menuPos % 5] : trigs[menuPos % 5];
    }
  }

  else {
    values[menuPos] += patch.encoder.Increment();
    values[menuPos] = values[menuPos] < 0.f ? 0.f : values[menuPos];
    values[menuPos] = values[menuPos] > 59.f ? 59.f : values[menuPos];
    inSubMenu = patch.encoder.RisingEdge() ? false : true;
  }

  // gate in
  if (patch.gateIns[0].Trig() || patch.gateIns[1].Trig()) {
    stepNumber++;
    stepNumber %= 5;
    trigOut = trigs[stepNumber];
  }

  //outputs
  analogWrite(PIN_PATCH_CV_1, round((values[stepNumber] / 12.f) * 51.2)); //51.2 = 256 / 5
  analogWrite(PIN_PATCH_CV_2, round((values[stepNumber] / 12.f) * 51.2));

  digitalWrite(PIN_PATCH_GATE_OUT, trigOut);
  trigOut = false;
}

void setup(){
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);

  pinMode(PIN_PATCH_GATE_OUT, OUTPUT);

  // init global vars
  stepNumber = 0;
  trigOut = false;
  menuPos = 0;
  inSubMenu = false;

  for (int i = 0; i < 5; i++) {
    values[i] = 0.f;
    trigs[i] = false;
  }

  oled.setFont(u8g2_font_t0_12_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();
  
  DAISY.begin(Callback);
}
void loop(){
  oled.clearBuffer();

  char cstr[15];
  sprintf(cstr, "!");
  oled.drawStr(25 * stepNumber, 53, cstr);

  // values and trigs
  for (int i = 0; i < 5; i++) {
    sprintf(cstr, "%d", values[i]);
    oled.drawStr(i * 25, 18, cstr);

    sprintf(cstr, (trigs[i % 5] ? "X" : "O") );
    oled.drawStr(i * 25, 38, cstr);
  }

  // cursor
  sprintf(cstr, (inSubMenu ? "@" : "o") );
  oled.drawStr( (menuPos % 5) * 25, (menuPos > 4) * 20 + 8, cstr);

  oled.sendBuffer();
}