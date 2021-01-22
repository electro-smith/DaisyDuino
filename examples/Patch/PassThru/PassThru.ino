// Title: PassThru
// Description: Passes in 1 to out 1 and in 2 to out 2
// Hardware: Daisy Patch
// Author: Stephen Hensley

#include "DaisyDuino.h"

size_t num_channels;

DaisyHardware hw;

void MyCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = in[chn][i];
    }
  }
}

void setup() {
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  pinMode(LED_BUILTIN, OUTPUT);
  DAISY.begin(MyCallback);
  // Debug Stuff for now
  Serial.begin(9600);
}

void loop() {
  // Do something else here like blink the LED
  delay(250);
  digitalToggle(LED_BUILTIN);
  Serial.println(num_channels);
}
