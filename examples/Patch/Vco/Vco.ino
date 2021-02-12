// Title: Vco
// Description: Vco with amplitude, waveforms, fine tune, and pitch
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/Vco/resources/Vco.png
//
// Controls
// Ctrl1: Pitch
// Ctrl2: Fine Tune
// Ctrl3: Waveform
// Ctrl4: Amplitude

#include "DaisyDuino.h"
#include <U8g2lib.h>

DaisyHardware hw;

DaisyHardware patch;
Oscillator osc;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);
int num_waves, num_channels;

static void AudioCallback(float **in, float **out, size_t size) {
  float sig, freq, amp;
  size_t wave;

  // Read Knobs
  freq = mtof(((1023 - analogRead(PIN_PATCH_CTRL_1)) / 10) + 10 +
              (1023 - analogRead(PIN_PATCH_CTRL_2)) / 146);
  wave = (1023 - analogRead(PIN_PATCH_CTRL_3)) / (1023 / num_waves);
  amp = (1023 - analogRead(PIN_PATCH_CTRL_4)) / 2048.f;

  //    freq = 440;
  // wave = 0;
  // amp = .5;

  for (size_t i = 0; i < size; i++) {

    // Set osc params
    osc.SetFreq(freq);
    osc.SetWaveform(wave);
    osc.SetAmp(amp);

    // Process
    sig = osc.Process();

    // Assign Synthesized Waveform to all four outputs.
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float samplerate;
  num_waves = Oscillator::WAVE_LAST - 1;
  hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();

  osc.Init(samplerate); // Init oscillator

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();

  DAISY.begin(AudioCallback);
}

void loop() { oled.drawString(6, 4, "VCO"); }