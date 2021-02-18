
#include "DaisyDuino.h"

#define NUM_CONTROLS 4

DaisyHardware hw;
ModalVoice modal;

uint8_t buttons[16];
// Use bottom row to set major scale
// Top row chromatic notes, and the inbetween notes are just the octave.
float scale[16] = {0.f, 2.f, 4.f, 5.f, 7.f, 9.f, 11.f, 12.f,
                   0.f, 1.f, 3.f, 0.f, 6.f, 8.f, 10.f, 0.0f};
float active_note = scale[0];

int8_t octaves = 0;

// Use two side buttons to change octaves.
float kvals[NUM_CONTROLS];

void AudioCallback(float **in, float **out, size_t size) {
  hw.ProcessDigitalControls();

  octaves += hw.buttons[0].RisingEdge() ? -1 : 0;
  octaves += hw.buttons[1].RisingEdge() ? 1 : 0;
  octaves = DSY_MIN(DSY_MAX(0, octaves), 4);

  modal.SetBrightness(kvals[0]);
  modal.SetStructure(kvals[1]);
  modal.SetDamping(kvals[2]);
  modal.SetAccent(kvals[3]);

  for (size_t i = 0; i < 16; i++) {
    if (hw.KeyboardRisingEdge(i) && i != 8 && i != 11 && i != 15) {
      modal.Trig();
      float m = (12.0f * octaves) + 24.0f + scale[i];
      modal.SetFreq(mtof(m));
    }
  }

  for (size_t i = 0; i < size; i++) {
    out[0][i] = out[1][i] = modal.Process();
  }
}

void UpdateLeds(float *knob_vals) {
  // knob_vals is exactly 8 members
  for (size_t i = 0; i < 8; i++) {
    float val = i < NUM_CONTROLS ? knob_vals[i] : 0.f;
    hw.SetKnobLed(i, val);
  }
  
  // white keys
  for (size_t i = 0; i < 8; i++) {
    hw.SetKeyboardLed(1, i, 1.f);
  }

  // black keys
  hw.SetKeyboardLed(0, 1, 1.f);
  hw.SetKeyboardLed(0, 2, 1.f);
  hw.SetKeyboardLed(0, 4, 1.f);
  hw.SetKeyboardLed(0, 5, 1.f);
  hw.SetKeyboardLed(0, 6, 1.f);
  
  hw.led_driver_.SwapBuffersAndTransmit();
}

void setup() {
  float sr;
  hw = DAISY.init(DAISY_FIELD, AUDIO_SR_48K);
  sr = DAISY.AudioSampleRate();

  // Initialize controls.
  octaves = 2;

  modal.Init(sr);

  DAISY.begin(AudioCallback);
}
void loop() {
  hw.ProcessAnalogControls();
  for (int i = 0; i < NUM_CONTROLS; i++) {
    kvals[i] = hw.GetKnobValue(i);
  }

  
  UpdateLeds(kvals);
  delay(6);
}