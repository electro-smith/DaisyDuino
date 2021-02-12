// Title: PolyOsc
// Description: Quad Multiwaveform Oscillator
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/PolyOsc/resources/PolyOsc.png
// Controls
// Ctrls 1-3: Oscillator 1-3 Pitch
// Ctrl 4: Master Pitch
// Encoder Turn: Global Waveform Select
// Audio Out 1-3: Oscillator 1-3 Output
// Audio Out 4: Mix Out

#include "DaisyDuino.h"
#include <U8g2lib.h>
#include <U8x8lib.h>

DaisyHardware patch;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

Oscillator osc[3];

std::string waveNames[5];

int waveform;
int final_wave;

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float mix = 0;
    // Process and output the three oscillators

    for (size_t chn = 0; chn < 4; chn++) {
      float sig = osc[chn].Process();
      mix += sig * .25f;
      out[chn][i] = sig;
    }

    // output the mixed oscillators
    out[3][i] = mix;
  }
}

void SetupOsc(float samplerate) {
  for (int i = 0; i < 3; i++) {
    osc[i].Init(samplerate);
    osc[i].SetAmp(.7);
  }
}

void SetupWaveNames() {
  waveNames[0] = "sine";
  waveNames[1] = "triangle";
  waveNames[2] = "saw";
  waveNames[3] = "ramp";
  waveNames[4] = "square";
}

void setup() {
  // put your setup code here, to run once:
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  waveform = 0;
  final_wave = Oscillator::WAVE_POLYBLEP_TRI;

  SetupOsc(samplerate);
  SetupWaveNames();

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();

  UpdateOled();

  DAISY.begin(AudioCallback);
}

void loop() { UpdateControls(); }

void UpdateControls() {
  patch.DebounceControls();

  // knobs
  float ctrl[4];
  ctrl[0] = (1023 - analogRead(PIN_PATCH_CTRL_1)) / 1023.f;
  ctrl[1] = (1023 - analogRead(PIN_PATCH_CTRL_2)) / 1023.f;
  ctrl[2] = (1023 - analogRead(PIN_PATCH_CTRL_3)) / 1023.f;
  ctrl[3] = (1023 - analogRead(PIN_PATCH_CTRL_4)) / 1023.f;

  for (int i = 0; i < 3; i++) {
    ctrl[i] += ctrl[3];
    ctrl[i] = ctrl[i] * 5.f;           // voltage
    ctrl[i] = powf(2.f, ctrl[i]) * 55; // Hz
  }

  // encoder
  int inc = patch.encoder.Increment();
  waveform += inc;
  waveform = (waveform % final_wave + final_wave) % final_wave;

  // Adjust oscillators based on inputs
  for (int i = 0; i < 3; i++) {
    osc[i].SetFreq(ctrl[i]);
    osc[i].SetWaveform((uint8_t)waveform);
  }

  if (inc != 0) {
    UpdateOled();
  }
}

void UpdateOled() {
  oled.clear();

  String str = "PolyOsc";
  char *cstr = &str[0];
  oled.drawString(0, 0, cstr);

  str = "wave:";
  oled.drawString(0, 3, cstr);

  cstr = &waveNames[waveform][0];
  oled.drawString(6, 3, cstr);
}
