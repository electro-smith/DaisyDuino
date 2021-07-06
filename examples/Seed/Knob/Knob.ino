// Title: Knob
// Description: Make an led brighter or darker with a knob
// Hardware: Daisy Seed
// Author: Ben Sergentanis
// Breadboard:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Knob/resources/Knob_bb.png
// Schematic:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Knob/resources/Knob_schem.png

uint8_t led_pin;
uint8_t knob_pin;

void setup() {
  led_pin = 28;
  knob_pin = A6;
  pinMode(led_pin, OUTPUT);
  pinMode(knob_pin, INPUT);
}

void loop() {
  // we map the 10 bit input value to a 8 bit PWM output
  analogWrite(led_pin, analogRead(knob_pin) / 4);
}
