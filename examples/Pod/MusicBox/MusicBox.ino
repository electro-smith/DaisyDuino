// Title: Music Box
// Description: Lovely melodies from random notes
// Hardware: Daisy Pod
// Author: Stephen Hensley
// Controls:
// Button 1: Trigger next note
// Knob 1: Decay time
// Knob 2: Reverb amount
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/pod/MusicBox/resources/MusicBox.png

#include "DaisyDuino.h"

DaisyHardware hw;
Oscillator osc;
Svf filt;
ReverbSc verb;
AdEnv env;

const static float scale[7] = {0, 2, 4, 5, 7, 9, 11};

static float get_new_note() {
  int32_t val, oct, idx;
  val = myrand() % 14;
  oct = val / 7;
  idx = val % 7;
  return scale[idx] + (12 * oct);
}

static float freq;
float sig, rawsig, filtsig, sendsig, wetvl, wetvr;
float vamt, dec, time;
static void audio(float **in, float **out, size_t size) {
  hw.DebounceControls();

  if (hw.buttons[0].RisingEdge()) {
    freq = mtof(48.0f + get_new_note());
    osc.SetFreq(freq);
    env.SetTime(ADENV_SEG_DECAY, dec);
    env.Trigger();
  }

  // Get Parameters
  dec = ((float)analogRead(PIN_POD_POT_1) / 1023.f) * 5 + .2; // exponential
  vamt = (float)analogRead(PIN_POD_POT_2) / 1023.f;

  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    // Process
    rawsig = osc.Process();
    sig = rawsig * env.Process();
    filt.Process(sig);
    filtsig = filt.Low();
    sendsig = filtsig * vamt;
    verb.Process(sendsig, sendsig, &wetvl, &wetvr);
    out[0][i] = (filtsig + (wetvl)) * 0.707f;
    out[1][i] = (filtsig + (wetvr)) * 0.707f;
  }
}

void InitSynth(float samplerate) {
  dec = 0.62;
  // Init Osc and Nse
  osc.Init(samplerate);
  osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
  osc.SetFreq(100.0f);
  osc.SetAmp(.5f);
  env.Init(samplerate);
  env.SetCurve(-15.0f);
  env.SetTime(ADENV_SEG_ATTACK, 0.002f);
  env.SetTime(ADENV_SEG_DECAY, 2.6f);
  env.SetMax(1.f);
  env.SetMin(0.f);
  filt.Init(samplerate);
  filt.SetRes(0.5f);
  filt.SetDrive(0.8f);
  filt.SetFreq(2400.0f);
  verb.Init(samplerate);
  verb.SetFeedback(0.87);
  verb.SetLpFreq(10000.0f);
}

void setup() {
  float samplerate, callback_rate;
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);

  hw.leds[0].Set(false, false, false);
  hw.leds[1].Set(false, false, false);

  samplerate = DAISY.get_samplerate();
  InitSynth(samplerate);

  DAISY.begin(audio);
}

void loop() {}