// Title: Lfo
// Description:
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/lfo/resources/lfo.png
//
// Controls
// Encoder Turn: Move cursor / Change Waveform
// Encoder Press: Select/Deselect Menu Item
// Ctrl1: Lfo 1 Freq
// Ctrl2: Lfo 1 Amp
// Ctrl3: Lfo 2 Freq
// Ctrl4: Lfo 2 Amp
// Cv Out 1: Lfo 1 Out
// Cv Out 2: Lfo 2 Out

#include "DaisyDuino.h"
#include <U8x8lib.h>

#include <Wire.h>

#define MAX_WAVE Oscillator::WAVE_POLYBLEP_TRI

DaisyHardware patch;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

bool menuSelect;
int lfoSelect;
String waveNames[5];

struct lfoStruct {
  Oscillator osc;
  uint8_t freqCtrl;
  uint8_t ampCtrl;
  int waveform;
  float amp;

  void Init(float samplerate, uint8_t freq, uint8_t amp) {
    osc.Init(samplerate);
    osc.SetAmp(1);
    waveform = 0;
    freqCtrl = freq;
    ampCtrl = amp;
  }

  void Update() {
    osc.SetFreq(((1023.f - analogRead(freqCtrl)) / 1023.f) * 35.f + 1.f);
    osc.SetWaveform(waveform);
    amp = ((1023.f - analogRead(ampCtrl)) / 1023.f) * 255.f;
  }

  void Process(uint8_t chn) {
    // write to the DAC
    analogWrite(chn, (osc.Process() + 1.f) * .5f * amp);
  }
};

lfoStruct lfos[2];

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    lfos[0].Process(PIN_PATCH_CV_1);
    lfos[1].Process(PIN_PATCH_CV_2);
  }
}

void SetupWaveNames() {
  waveNames[0] = "sine";
  waveNames[1] = "tri";
  waveNames[2] = "saw";
  waveNames[3] = "ramp";
  waveNames[4] = "square";
}

void setup() {
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  // init the lfos
  lfos[0].Init(samplerate, PIN_PATCH_CTRL_1, PIN_PATCH_CTRL_2);
  lfos[1].Init(samplerate, PIN_PATCH_CTRL_3, PIN_PATCH_CTRL_4);

  lfoSelect = 0;
  menuSelect = false;

  SetupWaveNames();

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();

  UpdateOled();

  DAISY.begin(AudioCallback);
}

void loop() { UpdateEncoder(); }

void UpdateOled() {
  oled.clear();

  oled.drawString(0, 0, "Dual Lfo");

  // cursor
  String str = menuSelect ? "@" : "o";
  char *ch = &str[0];
  oled.drawString(1 + lfoSelect * 7, 2, ch);

  // waveforms
  for (int i = 0; i < 2; i++) {
    ch = &waveNames[lfos[i].waveform][0];
    oled.drawString(i * 7, 4, ch);
  }
}

void UpdateEncoder() {
  patch.DebounceControls();

  bool edge = patch.encoder.RisingEdge();

  // annoying menu stuff
  if (edge) {
    menuSelect = !menuSelect;
  }

  int inc = patch.encoder.Increment();

  if (menuSelect) {
    lfos[lfoSelect].waveform += inc;
    lfos[lfoSelect].waveform =
        (lfos[lfoSelect].waveform % MAX_WAVE + MAX_WAVE) % MAX_WAVE;
  } else {
    lfoSelect += inc;
    lfoSelect = (lfoSelect % 2 + 2) % 2;
  }

  lfos[0].Update();
  lfos[1].Update();

  if (edge || inc != 0) {
    UpdateOled();
    // oled.clear();
  }
}
