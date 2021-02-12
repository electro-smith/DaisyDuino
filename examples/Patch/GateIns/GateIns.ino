// Title: GateIns
// Description: Demonstrates usage of the gate inputs
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Controls
// Gate In 1: When held on, oled displays "ON", otherwise oled displays "OFF"
// Gate In 2: On a trigger aka rising edge, oled briefly flashes "BANG!"

#include "DaisyDuino.h"
#include <U8g2lib.h>

U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

DaisyHardware hw;
bool state;

void setup() {
  hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);

  state = false;

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();
  oled.drawString(6, 4, "OFF");
}

void loop() {
  hw.DebounceControls();
  bool newState = hw.gateIns[0].State();
  if (newState != state) {
    state = newState;
    oled.clear();
    if (state)
      oled.drawString(6, 4, "ON");
    else
      oled.drawString(6, 4, "OFF");
  }

  if (hw.gateIns[1].Trig()) {
    oled.drawString(6, 2, "BANG!");
    delay(20);

    oled.clear();

    if (state)
      oled.drawString(6, 4, "ON");
    else
      oled.drawString(6, 4, "OFF");
  }
}
