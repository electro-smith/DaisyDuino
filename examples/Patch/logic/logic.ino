// Ctrl 1-4: Gate in 1-4
// Encoder turn: Navigate menu
// Encoder Press: Invert input, select gate type
// CV Out 1-2: Gate outputs

#include "DaisyDuino.h"
#include <string>
#include <U8g2lib.h>
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);

DaisyHardware patch;

struct gate {
  int gateType;
  bool Process(bool a, bool b) {
    switch (gateType) {
    case 0:
      return a && b; // AND
    case 1:
      return a || b; // OR
    case 2:
      return (a && !b) || (!a && b); // XOR
    case 3:
      return !(a && b); // NAND
    case 4:
      return !(a || b); // NOR
    case 5:
      return (a && b) || (!a && !b); // XNOR
    }
    return false;
  }
};

bool inputs[4];
gate gates[2];
std::string gateNames[6];
bool isInverted[4];

// menu logic
int menuPos;      // 6 positions
bool inSubMenu;   // only on gate types
int cursorPos[6]; // x positions for the OLED cursor

int mod(int x, int m) { return (x % m + m) % m; }

void ProcessControls();
void ProcessOutputs();

void InitGateNames() {
  gateNames[0] = " AND ";
  gateNames[1] = " OR  ";
  gateNames[2] = " XOR ";
  gateNames[3] = " NAND";
  gateNames[4] = " NOR ";
  gateNames[5] = " XNOR";
}

void InitCursorPos() {
  cursorPos[0] = 5;
  cursorPos[1] = 23;
  cursorPos[2] = 48;
  cursorPos[3] = 75;
  cursorPos[4] = 92;
  cursorPos[5] = 117;
}

//just using this so the controls interrupt the oled
static void AudioCallback(float **in, float **out, size_t size) {
  ProcessControls();
  ProcessOutputs();
}


void setup(){
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);

  InitGateNames();
  InitCursorPos();

  // both init to AND
  gates[0].gateType = gates[1].gateType = 0;

  // no inputs are inverted
  isInverted[0] = isInverted[1] = false;
  isInverted[2] = isInverted[3] = false;

  oled.setFont(u8g2_font_t0_12_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();

  DAISY.begin(AudioCallback);
}

void loop() 
{
  oled.clearBuffer();

  std::string str;
  char *cstr = &str[0];

  str = "Logic";
  oled.drawStr(0, 8, cstr);

  // dashes or spaces, depending on negation
  std::string negStr[4];
  for (int i = 0; i < 4; i++) {
    negStr[i] = isInverted[i] ? "-" : " ";
  }

  // gates and inputs, with negations
  str = negStr[0] + "1" + gateNames[gates[0].gateType] + negStr[1] + "2";
  oled.drawStr(0, 43, cstr);

  str = negStr[2] + "3" + gateNames[gates[1].gateType] + negStr[3] + "4";
  oled.drawStr(70, 43, cstr);

  // Cursor
  str = inSubMenu ? "@" : "o";
  oled.drawStr(cursorPos[menuPos], 33, cstr);

  oled.sendBuffer();
}

void ProcessIncrement(int increment) {
  // change gate type
  if (inSubMenu) {
    if (menuPos == 1) {
      gates[0].gateType += increment;
      gates[0].gateType = mod(gates[0].gateType, 6);
    }

    else {
      gates[1].gateType += increment;
      gates[1].gateType = mod(gates[1].gateType, 6);
    }
  }

  // move through the menu
  else {
    menuPos += increment;
    menuPos = mod(menuPos, 6);
  }
}

void ProcessRisingEdge() {
  switch (menuPos) {
  // flip inversions
  case 0:
    isInverted[0] = !isInverted[0];
    break;
  case 2:
    isInverted[1] = !isInverted[1];
    break;
  case 3:
    isInverted[2] = !isInverted[2];
    break;
  case 5:
    isInverted[3] = !isInverted[3];
    break;

  // enter/exit submenu
  case 1:
  case 4:
    inSubMenu = !inSubMenu;
  }
}

void ProcessEncoder() {
  ProcessIncrement(patch.encoder.Increment());

  if (patch.encoder.RisingEdge()) {
    ProcessRisingEdge();
  }
}

void ProcessControls() {
  patch.DebounceControls();

  int ctrls[4] = {PIN_PATCH_CTRL_1, PIN_PATCH_CTRL_2, PIN_PATCH_CTRL_3, PIN_PATCH_CTRL_4};

  // inputs
  for (int i = 0; i < 4; i++) {    
    inputs[i] = analogRead(ctrls[i]) < 204; //ctrls read from 1023 - 0

    // invert the input if isInverted says so
    inputs[i] = isInverted[i] ? !inputs[i] : inputs[i];
  }

  ProcessEncoder();
}

void ProcessOutputs() {
  analogWrite(PIN_PATCH_CV_1, gates[0].Process(inputs[0], inputs[1]) * 255.f);
  analogWrite(PIN_PATCH_CV_2, gates[1].Process(inputs[2], inputs[3]) * 255.f);
}