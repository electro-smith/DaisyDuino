// Title: QuadEnvelope
// Description: Quad VCA with built in AD envelopes
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/QuadEnvelope/resources/QuadEnvelope.png
// Controls
//
// Gate In 1/2 Trigger Envelope 1/2
// CV Out: Envelope CV Out
//
// Audio In 1/2 : VCA In 1/2, controlled by envelope 1
// Audio In 3/4 : VCA In 3/4, controlled by envelope 2
// Audio Out 1-4: VCA Out 1-4
//
// Encoder Press: Change Control mode
//
// attack/decay mode: ctrl 1-2 = env 1 attack decay
// ctrl 3-4 = env 2 attack decay
//
// Curve mode: ctrl 1 = env 1 curve
// ctrl 3 = env 2 curve

#include "DaisyDuino.h"
#include <U8g2lib.h>

DaisyHardware patch;

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);

int curveTimeMode;

struct envStruct {
  AdEnv env;
  uint8_t attackPin;
  uint8_t decayPin;
  uint8_t curvePin;
  GateIn *gatein;

  float envSig;

  void Init(float samplerate, uint8_t one, uint8_t two, GateIn *gate) {
    attackPin = one;
    decayPin = two;
    curvePin = one;
    gatein = gate;

    env.Init(samplerate);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(0);
  }

  void Process() {
    if (curveTimeMode == 0) {
      env.SetTime(ADENV_SEG_ATTACK,
                  ((1023.f - analogRead(attackPin)) / 1023.f) + .01);
      env.SetTime(ADENV_SEG_DECAY,
                  ((1023.f - analogRead(decayPin)) / 1023.f) + .01);
    } else {
      env.SetCurve(((1023.f - analogRead(curvePin)) / 1023.f) * 20 - 10);
    }

    if (gatein->Trig()) {
      env.Trigger();
    }
  }
};

envStruct envelopes[2];

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // Get the next envelope samples
    envelopes[0].envSig = envelopes[0].env.Process();
    envelopes[1].envSig = envelopes[1].env.Process();

    for (size_t chn = 0; chn < 2; chn++) {
      // The envelopes effect the outputs in pairs
      out[chn * 2][i] = in[chn * 2][i] * envelopes[chn].envSig;
      out[chn * 2 + 1][i] = in[chn * 2 + 1][i] * envelopes[chn * 2 + 1].envSig;
    }
  }
}

void setup() {
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  envelopes[0].Init(samplerate, PIN_PATCH_CTRL_1, PIN_PATCH_CTRL_2,
                    &patch.gateIns[0]);
  envelopes[1].Init(samplerate, PIN_PATCH_CTRL_3, PIN_PATCH_CTRL_4,
                    &patch.gateIns[1]);
  curveTimeMode = 0;

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();
  UpdateOled();

  DAISY.begin(AudioCallback);
}

void loop() {
  ProcessControls();

  // Send the latest envelope values to the CV outs
  analogWrite(PIN_PATCH_CV_1, envelopes[0].envSig * 255.f);
  analogWrite(PIN_PATCH_CV_2, envelopes[1].envSig * 255.f);
}

void UpdateOled() {
  oled.clear();

  oled.drawString(0, 0, "env1");

  oled.drawString(7, 0, "env2");

  String str = "";
  // curve or attack/decay mode
  if (curveTimeMode) {
    str = "curve";
  } else {
    str = "attack/decay";
  }

  char *cstr = &str[0];
  oled.drawString(0, 3, cstr);
}

void ProcessControls() {
  patch.DebounceControls();

  envelopes[0].Process();
  envelopes[1].Process();

  // encoder
  int edge = patch.encoder.RisingEdge();
  curveTimeMode += edge;
  curveTimeMode = curveTimeMode % 2;

  if (edge != 0) {
    UpdateOled();
  }
}
