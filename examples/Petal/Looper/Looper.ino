#include "DaisyDuino.h"

#define MAX_SIZE (48000 * 60 * 5) // 5 minutes of floats at 48 khz

static DaisyHardware petal;

bool first = true; // first loop (sets length)
bool rec = false;  // currently recording
bool play = false; // currently playing

int pos = 0;
float DSY_SDRAM_BSS buf[MAX_SIZE];
int mod = MAX_SIZE;
int len = 0;
float drywet = 0;
bool res = false;

void ResetBuffer();
void Controls();

void NextSamples(float &output, float *in, size_t i);

static void AudioCallback(float *in, float *out, size_t size) {
  float output = 0;

  Controls();

  for (size_t i = 0; i < size; i += 2) {
    NextSamples(output, in, i);

    // left and right outs
    out[i] = out[i + 1] = output;
  }
}

void setup() {
  // initialize petal hardware and oscillator daisysp module

  petal = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);
  ResetBuffer();

  rec = false;
  play = false;

  petal.ClearLeds();

  // start callback
  DAISY.begin(AudioCallback);
}
void loop() {
  // leds
  petal.SetFootswitchLed(1, play);
  petal.SetFootswitchLed(0, rec);
  petal.UpdateLeds();
  delay(6);
}

// Resets the buffer
void ResetBuffer() {
  play = false;
  rec = false;
  first = true;
  pos = 0;
  len = 0;
  for (int i = 0; i < mod; i++) {
    buf[i] = 0;
  }
  mod = MAX_SIZE;
}

void UpdateButtons() {
  // switch1 pressed
  if (petal.buttons[0].RisingEdge()) {
    if (first && rec) {
      first = false;
      mod = len;
      len = 0;
    }

    res = true;
    play = true;
    rec = !rec;
  }

  // switch1 held
  if (petal.buttons[0].TimeHeldMs() >= 1000 && res) {
    ResetBuffer();
    res = false;
  }

  // switch2 pressed and not empty buffer
  if (petal.buttons[1].RisingEdge() && !(!rec && first)) {
    play = !play;
    rec = false;
  }
}

// Deals with analog controls
void Controls() {
  petal.ProcessAllControls();
  
  drywet = petal.controls[0].Value();

  UpdateButtons();
}

void WriteBuffer(float *in, size_t i) {
  buf[pos] = buf[pos] * 0.5 + in[i] * 0.5;
  if (first) {
    len++;
  }
}

void NextSamples(float &output, float *in, size_t i) {
  if (rec) {
    WriteBuffer(in, i);
  }

  output = buf[pos];

  // automatic looptime
  if (len >= MAX_SIZE) {
    first = false;
    mod = MAX_SIZE;
    len = 0;
  }

  if (play) {
    pos++;
    pos %= mod;
  }

  if (!rec) {
    output = output * drywet + in[i] * (1 - drywet);
  }
}