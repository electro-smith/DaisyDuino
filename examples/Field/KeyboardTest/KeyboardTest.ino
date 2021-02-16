#include "DaisyDuino.h"

#define NUM_VOICES 16

DaisyHardware hw;

struct voice {
  void Init(float samplerate) {
    osc_.Init(samplerate);
    amp_ = 0.0f;
    osc_.SetAmp(0.7f);
    osc_.SetWaveform(daisysp::Oscillator::WAVE_POLYBLEP_SAW);
    on_ = false;
  }
  float Process() {
    float sig;
    amp_ += 0.0025f * ((on_ ? 1.0f : 0.0f) - amp_);
    sig = osc_.Process() * amp_;
    return sig;
  }
  void set_note(float nn) { osc_.SetFreq(daisysp::mtof(nn)); }

  daisysp::Oscillator osc_;
  float amp_, midibase_;
  bool on_;
};

voice v[NUM_VOICES];
uint8_t buttons[16];
// Use bottom row to set major scale
// Top row chromatic notes, and the inbetween notes are just the octave.
float scale[16] = {0.f, 2.f, 4.f, 5.f, 7.f, 9.f, 11.f, 12.f,
                   0.f, 1.f, 3.f, 0.f, 6.f, 8.f, 10.f, 0.0f};
float active_note = scale[0];

int8_t octaves = 0;

static daisysp::ReverbSc verb;
// Use two side buttons to change octaves.
float kvals[8];
float cvvals[4];

bool use_verb;

void AudioCallback(float *in, float *out, size_t size) {
  hw.ProcessAllControls();

  bool trig;
  trig = false;
  if (hw.buttons[0].RisingEdge()) {
    octaves -= 1;
    trig = true;
  }
  if (hw.buttons[1].RisingEdge()) {
    octaves += 1;
    trig = true;
  }
  use_verb = true;

  if (octaves < 0)
    octaves = 0;
  if (octaves > 4)
    octaves = 4;

  if (trig) {
    for (int i = 0; i < NUM_VOICES; i++) {
      v[i].set_note((12.0f * octaves) + 24.0f + scale[i]);
    }
  }
  for (size_t i = 0; i < 16; i++) {
    v[i].on_ = hw.KeyboardState(i);
  }

  float sig, send;
  float wetl, wetr;
  for (size_t i = 0; i < size; i += 2) {
    sig = 0.0f;
    for (int i = 0; i < NUM_VOICES; i++) {
      if (i != 8 && i != 11 && i != 15)
        sig += v[i].Process();
    }
    send = sig * 0.35f;
    verb.Process(send, send, &wetl, &wetr);
    //        wetl = wetr = sig;
    if (!use_verb)
      wetl = wetr = 0.0f;
    out[i] = (sig + wetl) * 0.5f;
    out[i + 1] = (sig + wetr) * 0.5f;
  }
}

void UpdateLeds(float *knob_vals) {
  for (size_t i = 0; i < 8; i++) {
    hw.SetKnobLed(i, knob_vals[i]);
  }
  // white keys
  for (size_t i = 0; i < 8; i++) {
    hw.SetKeyboardLed(1, i, 1.f);
  }

  // black keys
  hw.SetKeyboardLed(0, 1, 1.f);
  hw.SetKeyboardLed(0, 2, 1.f);
  hw.SetKeyboardLed(0, 4, 1.f);
  hw.SetKeyboardLed(0, 5, 1.f);
  hw.SetKeyboardLed(0, 6, 1.f);

  hw.UpdateLeds();
}

void setup() {
  float sr;
  hw = DAISY.init(DAISY_FIELD, AUDIO_SR_48K);
  sr = DAISY.AudioSampleRate();
  // Initialize controls.
  octaves = 2;
  for (int i = 0; i < NUM_VOICES; i++) {
    v[i].Init(sr);
    v[i].set_note((12.0f * octaves) + 24.0f + scale[i]);
  }

  verb.Init(sr);
  verb.SetFeedback(0.94f);
  verb.SetLpFreq(8000.0f);

  DAISY.begin(AudioCallback);
}

void loop() {
  UpdateLeds(kvals);

  for (int i = 0; i < 8; i++) {
    kvals[i] = hw.GetKnobValue(i);
    if (i < 4) {
      cvvals[i] = hw.cv[i].Process();
    }
  }

  analogWrite(PIN_FIELD_DAC_1, hw.GetKnobValue(0) * 255);
  analogWrite(PIN_FIELD_DAC_2, hw.GetKnobValue(1) * 255);
}