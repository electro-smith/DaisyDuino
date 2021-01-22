//Ctrl 1-3: Delay time 1-3
//Ctrl 4: Feedback amount all
//Encoder: Dry/wet
//In 1: In all
//Out 1-3: Delay 1-3 out
//Out 4: Mix out.

#include "DaisyDuino.h"
#include <string>
#include <U8g2lib.h>
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);

#define MAX_DELAY static_cast<size_t>(48000 * 1.f)

DaisyHardware patch;

float samplerate;

DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delMems[3];

struct del {
  DelayLine<float, MAX_DELAY> *del;
  float currentDelay;
  float delayTarget;

  float Process(float feedback, float in) {
    // set delay times
    fonepole(currentDelay, delayTarget, .0002f);
    del->SetDelay(currentDelay);

    float read = del->Read();
    del->Write((feedback * read) + in);

    return read;
  }
};

del delays[3];

float feedback;
int drywet;

void ProcessControls();

static void AudioCallback(float **in, float **out, size_t size) {
  ProcessControls();

  for (size_t i = 0; i < size; i++) {
    float mix = 0;
    float fdrywet = (float)drywet / 100.f;

    // update delayline with feedback
    for (int d = 0; d < 3; d++) {
      float sig = delays[d].Process(feedback, in[0][i]);
      mix += sig;
      // out[d][i] = sig * fdrywet + (1.f - fdrywet) * in[0][i];
      out[d][i] = sig;
    }

    // apply drywet and attenuate
    mix = fdrywet * mix * .3f + (1.0f - fdrywet) * in[0][i];
    out[3][i] = mix;
  }
}

void InitDelays() {
  for (int i = 0; i < 3; i++) {
    // Init delays
    delMems[i].Init();
    delays[i].del = &delMems[i];
  }
}

void setup(){
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  InitDelays();

  drywet = 50;
  
  oled.setFont(u8g2_font_t0_12_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();
  
  DAISY.begin(AudioCallback);
}

void loop(){
  oled.clearBuffer();

  std::string str = "Multi Delay";
  char *cstr = &str[0];
  oled.drawStr(0,8,cstr);

  str = "Dry/Wet:  ";
  oled.drawStr(0,38,cstr);

  str = std::to_string(drywet);
  oled.drawStr(60, 38, cstr);

  oled.sendBuffer();
}

void ProcessControls() {
  patch.DebounceControls();

  int ctrls[3] = {PIN_PATCH_CTRL_1, PIN_PATCH_CTRL_2, PIN_PATCH_CTRL_3};
  
  // knobs
  for (int i = 0; i < 3; i++) {
    float val = (1023.f - analogRead(ctrls[i])) / 1023.f;
    float m = (float)MAX_DELAY - .05f * samplerate;
    delays[i].delayTarget = val * m + .05 * samplerate; //.05 * sr to MAX_DELAY
  }
  feedback = (1023.f - analogRead(PIN_PATCH_CTRL_4)) / 1023.f;

  // encoder
  drywet += 5 * patch.encoder.Increment();
  drywet > 100 ? drywet = 100 : drywet = drywet;
  drywet < 0 ? drywet = 0 : drywet = drywet;
}