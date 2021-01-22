
#include "DaisyDuino.h"
#include <string>
#include <U8g2lib.h>
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);

DaisyHardware patch;
int freqs[16];

int bank;

struct ConditionalUpdate {
  float oldVal = 0;

  bool Process(float newVal) {
    if (abs(newVal - oldVal) > .04) {
      oldVal = newVal;
      return true;
    }

    return false;
  }
};

ConditionalUpdate condUpdates[4];

struct Filter {
  Svf filt;
  float amp;

  void Init(float samplerate, float freq) {
    filt.Init(samplerate);
    filt.SetRes(1);
    filt.SetDrive(.002);
    filt.SetFreq(freq);
    amp = .5f;
  }

  float Process(float in) {
    filt.Process(in);
    return filt.Peak() * amp;
  }
};

Filter filters[16];

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float sig = 0.f;
    for (int j = 0; j < 16; j++) {
      sig += filters[j].Process(in[0][i]);
    }
    sig *= .06;

    out[0][i] = out[1][i] = out[2][i] = out[3][i] = sig;
  }
}

void InitFreqs() {
  freqs[0] = 50;
  freqs[1] = 75;
  freqs[2] = 110;
  freqs[3] = 150;
  freqs[4] = 220;
  freqs[5] = 350;
  freqs[6] = 500;
  freqs[7] = 750;
  freqs[8] = 1100;
  freqs[9] = 1600;
  freqs[10] = 2200;
  freqs[11] = 3600;
  freqs[12] = 5200;
  freqs[13] = 7500;
  freqs[14] = 11000;
  freqs[15] = 15000;
}

void InitFilters(float samplerate) {
  for (int i = 0; i < 16; i++) {
    filters[i].Init(samplerate, freqs[i]);
  }
}

void UpdateOled();
void UpdateControls();

void setup(){
  float samplerate;
  patch = DAISY.init(
      DAISY_PATCH, AUDIO_SR_48K); // Initialize hardware (daisy seed, and patch)
  samplerate = DAISY.get_samplerate();

  InitFreqs();
  InitFilters(samplerate);
  bank = 0;

  oled.setFont(u8g2_font_t0_12_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();

  DAISY.begin(AudioCallback);
}

void loop(){
  UpdateOled();
  UpdateControls();
}

void UpdateOled() {
  oled.clearBuffer();

  std::string str = "Filter Bank";
  char *cstr = &str[0];
  oled.drawStr(0,8,cstr);

  str = "";
  for (int i = 0; i < 2; i++) {
    str += std::to_string(freqs[i + 4 * bank]);
    str += "  ";
  }

  oled.drawStr(0,33,cstr);


  str = "";
  for (int i = 2; i < 4; i++) {
    str += std::to_string(freqs[i + 4 * bank]);
    str += "  ";
  }

  oled.drawStr(0,43,cstr);

  oled.sendBuffer();

  delay(5);
}

void UpdateControls() {
  patch.DebounceControls();

  // encoder
  bank += patch.encoder.Increment();
  bank = (bank % 4 + 4) % 4;

  bank = patch.encoder.RisingEdge() ? 0 : bank;

  int ctrlNums[4] = {PIN_PATCH_CTRL_1, PIN_PATCH_CTRL_2, PIN_PATCH_CTRL_3, PIN_PATCH_CTRL_4};
  // controls
  for (int i = 0; i < 4; i++) {
    float val = (1023.f - analogRead(ctrlNums[i])) / 1023.f;
    if (condUpdates[i].Process(val)) {
      filters[i + bank * 4].amp = val;
    }
  }
}