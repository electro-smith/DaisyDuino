// Title: Noise
// Description: Whitenoise with resonant Highpass and Lowpass filters
// Hardware: Daisy Patch
// Author: Ben Sergentanis
https
    : // raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/Noise/resources/Noise.png
// Controls
// Ctrl 1: Lowpass Cutoff
// Ctrl 2: Lowpass Resonance
// Ctrl 3: Highpass Cutoff
// Ctrl 4: Highpass Resonance

#include "DaisyDuino.h"
#include <U8g2lib.h>

      DaisyHardware patch;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);
int num_channels;

WhiteNoise noise;

struct filter {
  Svf filt;

  enum mode {
    LO,
    HI,
    BAND,
    NOTCH,
    PEAK,
    LAST,
  };
  mode filterMode;

  uint8_t cutKnob_, resKnob_;

  void Init(float samplerate, mode setMode, uint8_t cutKnob, uint8_t resKnob) {
    filt.Init(samplerate);
    filterMode = setMode;

    cutKnob_ = cutKnob;
    resKnob_ = resKnob;
  }

  void UpdateControls() {
    filt.SetFreq(((1023.f - analogRead(cutKnob_)) / 1023.f) * 12000 + 20);
    filt.SetRes(0.3 + ((1023.f - analogRead(resKnob_)) / 1023.f) * 0.7);
  }

  float Process(float in) {
    filt.Process(in);
    switch (filterMode) {
    case mode::LO:
      return filt.Low();
    case mode::HI:
      return filt.High();
    case mode::BAND:
      return filt.Band();
    case mode::NOTCH:
      return filt.Notch();
    case mode::PEAK:
      return filt.Peak();
    default:
      break;
    }
    return 0.f;
  }
};

filter lowpass, highpass;

static void AudioCallback(float **in, float **out, size_t size) {
  lowpass.UpdateControls();
  highpass.UpdateControls();

  for (size_t i = 0; i < size; i++) {
    float sig = noise.Process();
    sig = lowpass.Process(sig);
    sig = highpass.Process(sig);

    for (size_t chn = 0; chn < 4; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  num_channels = patch.num_channels;
  samplerate = DAISY.get_samplerate();

  noise.Init();
  lowpass.Init(samplerate, filter::mode::LO, PIN_PATCH_CTRL_1,
               PIN_PATCH_CTRL_2);
  highpass.Init(samplerate, filter::mode::HI, PIN_PATCH_CTRL_3,
                PIN_PATCH_CTRL_4);

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();
  oled.drawString(6, 4, "Noise");

  DAISY.begin(AudioCallback);
}

void loop() {}
