// Title: Button
// Description: Turn an led on and off with a button
// Hardware: DaisyHardware
// Author: Ben Sergentanis
// Breadboard:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Button/resources/Button_bb.png
// Schematic:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Button/resources/Button_schem.png

#include "DaisyDuino.h"

Switch button;

void setup() {
  // setup the button
  // update at 1kHz, no invert, on pin 28
  button.Init(1000, true, 28);

  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  // Debounce the button
  button.Debounce();

  // If the button is pressed, turn the LED on
  digitalWrite(LED_BUILTIN, button.Pressed());

  // wait 1 ms
  delay(1);
}