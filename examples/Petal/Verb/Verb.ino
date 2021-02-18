
#include "DaisyDuino.h"

// Declare a local daisy_petal for hardware access
DaisyHardware hw;

Parameter vtime, vfreq, vsend;
bool bypass;
ReverbSc verb;

// This runs at a fixed rate, to prepare audio samples
void callback(float *in, float *out, size_t size) {
  float dryl, dryr, wetl, wetr, sendl, sendr;
  hw.ProcessDigitalControls();
  verb.SetFeedback(vtime.Process());
  verb.SetLpFreq(vfreq.Process());
  vsend.Process(); // Process Send to use later
  // bypass = hw.buttons[DaisyHardware::SW_5].Pressed();
  if (hw.buttons[0].RisingEdge())
    bypass = !bypass;
  for (size_t i = 0; i < size; i += 2) {
    dryl = in[i];
    dryr = in[i + 1];
    sendl = dryl * vsend.Value();
    sendr = dryr * vsend.Value();
    verb.Process(sendl, sendr, &wetl, &wetr);
    if (bypass) {
      out[i] = in[i];         // left
      out[i + 1] = in[i + 1]; // right
    } else {
      out[i] = dryl + wetl;
      out[i + 1] = dryr + wetr;
    }
  }
}

void setup() {
  float samplerate;

  hw = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);
  samplerate = DAISY.AudioSampleRate();

  vtime.Init(hw.controls[0], 0.6f, 0.999f, Parameter::LOGARITHMIC);
  vfreq.Init(hw.controls[1], 500.0f, 20000.0f, Parameter::LOGARITHMIC);
  vsend.Init(hw.controls[2], 0.0f, 1.0f, Parameter::LINEAR);
  verb.Init(samplerate);

  DAISY.begin(callback);
}
void loop() {
  // Do Stuff InfInitely Here
  delay(10);
  hw.ClearLeds();
  hw.SetFootswitchLed(0, bypass ? 0.0f : 1.0f);
  hw.UpdateLeds();
}
