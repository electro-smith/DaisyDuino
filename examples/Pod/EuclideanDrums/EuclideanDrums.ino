// encoder turn = snare or kick. Led 2 green = kick, blue = snare
// Knob 1 Pattern density
// Knob 2 Pattern Length. 1-32 steps
// Buttons 1-2. Hold to increase / decrease tempo.
// Led 1 = tempo. Cycles through colors as tempo changes.

#include "DaisyDuino.h"

#define UINT32_MSB 0x80000000U
#define MAX_LENGTH 32U

DaisyHardware hardware;

Oscillator osc;
WhiteNoise noise;

AdEnv kickVolEnv, kickPitchEnv, snareEnv;
Metro tick;

bool kickSeq[MAX_LENGTH];
bool snareSeq[MAX_LENGTH];
uint8_t kickStep = 0;
uint8_t snareStep = 0;

void ProcessTick();
void ProcessControls();

void AudioCallback(float **in, float **out, size_t size) {
  float osc_out, noise_out, snr_env_out, kck_env_out, sig;

  ProcessTick();

  ProcessControls();

  // audio
  for (size_t i = 0; i < size; i ++) {
    snr_env_out = snareEnv.Process();
    kck_env_out = kickVolEnv.Process();

    osc.SetFreq(kickPitchEnv.Process());
    osc.SetAmp(kck_env_out);
    osc_out = osc.Process();

    noise_out = noise.Process();
    noise_out *= snr_env_out;

    sig = .5 * noise_out + .5 * osc_out;

    out[0][i] = out[1][i] = sig;
  }
}

void SetupDrums(float samplerate) {
  osc.Init(samplerate);
  osc.SetWaveform(Oscillator::WAVE_TRI);
  osc.SetAmp(1);

  noise.Init();

  snareEnv.Init(samplerate);
  snareEnv.SetTime(ADENV_SEG_ATTACK, .01);
  snareEnv.SetTime(ADENV_SEG_DECAY, .2);
  snareEnv.SetMax(1);
  snareEnv.SetMin(0);

  kickPitchEnv.Init(samplerate);
  kickPitchEnv.SetTime(ADENV_SEG_ATTACK, .01);
  kickPitchEnv.SetTime(ADENV_SEG_DECAY, .05);
  kickPitchEnv.SetMax(400);
  kickPitchEnv.SetMin(50);

  kickVolEnv.Init(samplerate);
  kickVolEnv.SetTime(ADENV_SEG_ATTACK, .01);
  kickVolEnv.SetTime(ADENV_SEG_DECAY, 1);
  kickVolEnv.SetMax(1);
  kickVolEnv.SetMin(0);
}

void SetSeq(bool *seq, bool in) {
  for (uint32_t i = 0; i < MAX_LENGTH; i++) {
    seq[i] = in;
  }
}

void setup() {
  hardware = DAISY.init(DAISY_POD, AUDIO_SR_48K);

  float samplerate = DAISY.get_samplerate();
  float callbackrate = DAISY.get_callbackrate();

  // setup the drum sounds
  SetupDrums(samplerate);

  tick.Init(5, callbackrate);

  SetSeq(snareSeq, 0);
  SetSeq(kickSeq, 0);

  DAISY.begin(AudioCallback);
}

void loop() {}

float snareLength = 32.f;
float kickLength = 32.f;

void IncrementSteps() {
  snareStep++;
  kickStep++;
  snareStep %= (int)snareLength;
  kickStep %= (int)kickLength;
}

void ProcessTick() {
  if (tick.Process()) {
    IncrementSteps();

    if (kickSeq[kickStep]) {
      kickVolEnv.Trigger();
      kickPitchEnv.Trigger();
    }

    if (snareSeq[snareStep]) {
      snareEnv.Trigger();
    }
  }
}

void SetArray(bool *seq, int arrayLen, float density) {
  SetSeq(seq, 0);

  int ones = (int)round(arrayLen * density);
  int zeros = arrayLen - ones;

  if (ones == 0)
    return;

  if (zeros == 0) {
    SetSeq(seq, 1);
    return;
  }

  int oneArr[ones];
  int zeroArr[ones];

  for (int i = 0; i < ones; i++) {
    oneArr[i] = zeroArr[i] = 0;
  }

  // how many zeroes per each one
  int idx = 0;
  for (int i = 0; i < zeros; i++) {
    zeroArr[idx] += 1;
    idx++;
    idx %= ones;
  }

  // how many ones remain
  int rem = 0;
  for (int i = 0; i < ones; i++) {
    if (zeroArr[i] == 0)
      rem++;
  }

  // how many ones on each prior one
  idx = 0;
  for (int i = 0; i < rem; i++) {
    oneArr[idx] += 1;
    idx++;
    idx %= ones - rem;
  }

  // fill the global seq
  idx = 0;
  for (int i = 0; i < (ones - rem); i++) {
    seq[idx] = 1;
    idx++;

    for (int j = 0; j < zeroArr[i]; j++) {
      seq[idx] = 0;
      idx++;
    }

    for (int j = 0; j < oneArr[i]; j++) {
      seq[idx] = 1;
      idx++;
    }
  }
}

uint8_t mode = 0;
void UpdateEncoder() {
  mode += hardware.encoder.Increment();
  mode = (mode % 2 + 2) % 2;
  hardware.leds[1].Set(0, !mode, mode);
}

void ConditionalParameter(float o, float n, float &param, float update) {
  if (abs(o - n) > 0.0005) {
    param = update;
  }
}

float snareAmount, kickAmount = 0.f;
float k1old, k2old = 0.f;
void UpdateKnobs() {
  float k1 = (float)analogRead(PIN_POD_POT_1) / 1023.f;
  float k2 = (float)analogRead(PIN_POD_POT_2) / 1023;

  if (mode) {
    ConditionalParameter(k1old, k1, snareAmount, k1);
    ConditionalParameter(k2old, k2, snareLength, k2 * 32.f);
  } else {
    ConditionalParameter(k1old, k1, kickAmount, k1);
    ConditionalParameter(k2old, k2, kickLength, k2 * 32.f);
  }

  k1old = k1;
  k2old = k2;
}

float tempo = 3;
void UpdateButtons() {
  if (hardware.buttons[1].Pressed()) {
    tempo += .003;
  }

  else if (hardware.buttons[0].Pressed()) {
    tempo -= .003;
  }

  tempo = std::fminf(tempo, 10.f);
  tempo = std::fmaxf(.5f, tempo);

  tick.SetFreq(tempo);
  int tmp = (int)(tempo * .6f) + 1;
  hardware.leds[0].Set(tmp & 1, tmp & 2, tmp & 4);
}

void UpdateVars() {
  SetArray(snareSeq, (int)round(snareLength), snareAmount);
  SetArray(kickSeq, (int)round(kickLength), kickAmount);
}

void ProcessControls() {
  hardware.DebounceControls();

  UpdateEncoder();

  UpdateKnobs();

  UpdateButtons();

  UpdateVars();
}