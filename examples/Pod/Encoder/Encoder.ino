// Title: Encoder
// Description: Sets leds based on encoder
// Hardware: Daisy Pod
// Author: Ben Sergentanis
// Controls:
// Encoder Turn: Change led color
// Encoder Press: Set leds off
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/pod/Encoder/resources/Encoder.png

#include "DaisyDuino.h"

uint8_t color;

DaisyHardware hw;

void setup() {
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);

  color = 0;
}

void SetColor() {
  hw.leds[0].Set((color & B100) == B100, (color & B010) == B010,
                 (color & B001) == B001);
  hw.leds[1].Set((color & B100) == B100, (color & B010) == B010,
                 (color & B001) == B001);
}

void loop() {
  hw.DebounceControls();

  if (hw.encoder.RisingEdge()) {
    color = 0;
  }

  color += hw.encoder.Increment();
  color = (color % 8 + 8) % 8;

  SetColor();
}