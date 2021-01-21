// Title: SimpleOscillator
// Description: Oscillator with pitch and waveforms
// Hardware: Daisy Pod
// Author: Stephen Hensley
// Controls:
// Knob 1: Oscillator pitch
// Encoder: Waveform select. Sine, tri, saw, square
// Button 1: Octave down
// Button 2: Octave up
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/pod/SimpleOscillator/resources/SimpleOscillator.png

#include "DaisyDuino.h"

#define NUM_WAVEFORMS 4

DaisyHardware hw;
Oscillator osc;

uint8_t waveforms[NUM_WAVEFORMS] = {
    Oscillator::WAVE_SIN,
    Oscillator::WAVE_TRI,
    Oscillator::WAVE_POLYBLEP_SAW,
    Oscillator::WAVE_POLYBLEP_SQUARE,
};

static float freq;
float sig;
static int waveform, octave;

static void AudioCallback(float **in, float **out, size_t size) {
  hw.DebounceControls();

  waveform += hw.encoder.Increment();
  waveform = (waveform % NUM_WAVEFORMS + NUM_WAVEFORMS) % NUM_WAVEFORMS;
  osc.SetWaveform(waveforms[waveform]);

  if (hw.buttons[1].RisingEdge())
    octave++;
  if (hw.buttons[0].RisingEdge())
    octave--;
  octave = DSY_CLAMP(octave, 0, 4);

  // convert MIDI to frequency and multiply by octave size
  freq = analogRead(PIN_POD_POT_1) / 1023.f;
  freq = mtof(freq * 127 + (octave * 12));
  osc.SetFreq(freq);

  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    // Process
    sig = osc.Process();
    out[0][i] = sig;
    out[1][i] = sig;
  }
}

void InitSynth(float samplerate) {
  osc.Init(samplerate);
  osc.SetAmp(1.f);

  waveform = 0;
  octave = 0;
}

void setup() {
  float samplerate, callback_rate;
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  hw.leds[0].Set(false, false, false);
  hw.leds[1].Set(false, false, false);

  InitSynth(samplerate);

  DAISY.begin(AudioCallback);
}

void loop() {}