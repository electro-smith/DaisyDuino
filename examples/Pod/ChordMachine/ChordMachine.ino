// Knob 1 = frequency
// Knob 2 = inversion
//Encoder turn = chord

//major, minor, aug, dim triads
//M7, m7, dom7, M/m7, Dim 7, half dim 7
// red, green, yellow, blue
// pink, aqua, white, - red, red red, green red

#include "DaisyDuino.h"

DaisyHardware hw;
Oscillator osc[4];
int notes[4];
int chord[10][3];
int chordNum = 0;

void UpdateControls();

static void AudioCallback(float **in, float **out, size_t size) {
  UpdateControls();

  for (int i = 0; i < 4; i++) {
    osc[i].SetFreq(mtof(notes[i]));
  }

  // Audio Loop
  for (size_t i = 0; i < size; i ++) {
    float sig = 0;
    for (int i = 0; i < 4; i++) {
      sig += osc[i].Process();
    }

    out[0][i] = out[1][i] = sig;
  }
}

void InitSynth(float samplerate) {
  // Init freq Parameter to knob1 using MIDI note numbers
  // min 10, max 127, curve linear

  for (int i = 0; i < 4; i++) {
    osc[i].Init(samplerate);
    osc[i].SetAmp(0.1f);
    osc[i].SetWaveform(Oscillator::WAVE_SIN);
    notes[i] = 30;
  }
}

void InitChords() {
  // Maj, min, Aug, Dim
  // Maj7, min7, dom7, min/Maj7
  // dim7, half dim7

  // set thirds
  for (int i = 0; i < 8; i++) {
    // every other chord, maj third, min third
    chord[i][0] = 3 + ((i + 1) % 2);
  }
  // min 3rds
  chord[8][0] = chord[9][0] = 3;

  // set fifths
  // perfect 5th
  chord[0][1] = chord[1][1] = chord[4][1] = chord[5][1] = chord[6][1] =
      chord[7][1] = 7;
  // diminished 5th
  chord[3][1] = chord[8][1] = chord[9][1] = 6;
  // augmented 5th
  chord[2][1] = 8;

  // set sevenths
  // triads (octave since triad has no 7th)
  chord[0][2] = chord[1][2] = chord[2][2] = chord[3][2] = 12;
  // major 7th
  chord[4][2] = chord[7][2] = 11;
  // minor 7th
  chord[5][2] = chord[6][2] = chord[9][2] = 10;
  // diminished 7th
  chord[8][2] = 9;
}

void setup() {
  float samplerate;

  // Init everything
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  InitSynth(samplerate);
  InitChords();

  // start callbacks
  DAISY.begin(AudioCallback);
}

void loop() {}

void UpdateEncoder() {
  if (hw.encoder.RisingEdge()) {
    chordNum = 0;
  }

  chordNum += hw.encoder.Increment();
  chordNum = (chordNum % 10 + 10) % 10;
}

void UpdateKnobs() {
  int freq = 127 * analogRead(PIN_POD_POT_1) / 1023;
  int inversion = 5 * analogRead(PIN_POD_POT_2) / 1023;  

  notes[0] = freq + (12 * (inversion >= 1));
  notes[1] = freq + chord[chordNum][0] + (12 * (inversion >= 2));
  notes[2] = freq + chord[chordNum][1] + (12 * (inversion >= 3));
  notes[3] = freq + chord[chordNum][2] + (12 * (inversion >= 4));
}

void UpdateLeds() {
  int tmp = chordNum + 1;
  hw.leds[0].Set(tmp & 1, tmp & 2, tmp & 4);
  hw.leds[1].Set(tmp & 8, 0, 0);
}

void UpdateControls() {
  hw.DebounceControls();

  UpdateEncoder();
  UpdateKnobs();
  UpdateLeds();
}