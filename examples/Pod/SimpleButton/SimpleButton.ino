// Title: SimpleButton
// Description: Turn leds on and off with buttons
// Hardware: Daisy Pod
// Author: Ben Sergentanis
// Controls:
// Button 1: Hold to turn led 1 On
// Button 2: Latching led 2 On/Off
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/pod/SimpleButton/resources/SimpleButton.png

#include "DaisyDuino.h"

DaisyHardware hw;

bool led2on;

void setup() {
  led2on = false;
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);
}

void loop() {
  delay(1);
  hw.DebounceControls();

  // using button1 as momentary switch for turning on/off led1
  bool onePressed = hw.buttons[0].Pressed();
  hw.leds[0].Set(onePressed, onePressed, onePressed);

  // using button2 as latching switch for toggling led2
  if (hw.buttons[1].RisingEdge())
    led2on = !led2on;

  hw.leds[1].Set(led2on, led2on, led2on);
}