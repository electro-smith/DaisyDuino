// Encoder press: Edit / Play mode.

//Edit mode:
//Encoder turn = step
//Knob 1 = Decay Time
//Knob 2 = Pitch
//Button 1 = Cycling. Led = Step #
//Button 2 = active.  Green = active

//Play mode:
//Encoder turn = waveform select
//Knob 1 = tempo
//Knob 2 = filter cutoff

#include "DaisyDuino.h"

DaisyHardware pod;

Oscillator osc;
AdEnv env;
Metro tick;
MoogLadder flt;

bool edit; // if we aren't editing, we're playing
bool editCycle;
uint8_t step;
uint8_t wave;
float dec[8];
float pitch[8];
bool active[8];
float env_out;

float pent[] = {110.f, 128.33f, 146.66f, 174.166f, 192.5f};

float oldk1, oldk2;
float tickFrequency, filterFrequency;
float k1, k2;

void Controls();

void NextSamples(float &sig);

static void AudioCallback(float **in, float **out, size_t size) {
  float sig;

  Controls();

  // Audio Loop
  sig = 0;
  for (size_t i = 0; i < size; i++) {
    NextSamples(sig);

    out[0][i] = sig;
    out[1][i] = sig;
  }
}

void setup() {
  float sample_rate;
  pod = DAISY.init(DAISY_POD, AUDIO_SR_48K);
  sample_rate = DAISY.get_samplerate();
  tickFrequency = 3.f;
  filterFrequency = 20000.f;
  k1 = k2 = 0.f;

  osc.Init(sample_rate);
  env.Init(sample_rate);
  tick.Init(3, sample_rate);
  flt.Init(sample_rate);

  // Osc parameters
  osc.SetWaveform(osc.WAVE_TRI);

  // Envelope parameters
  env.SetTime(ADENV_SEG_ATTACK, 0.01);
  env.SetMin(0.0);
  env.SetMax(1);

  // Set filter parameters
  flt.SetFreq(20000);
  flt.SetRes(0.7);

  // Global variables
  oldk1 = oldk2 = 0;
  edit = true;
  editCycle = false;
  step = 0;
  env_out = 0;
  wave = 2;

  for (int i = 0; i < 8; i++) {
    dec[i] = .5;
    active[i] = false;
    pitch[i] = 110;
  }

  DAISY.begin(AudioCallback);
}

void loop() {}

void ConditionalParameter(float o, float n, float &param, float update) {
  if (abs(o - n) > 0.0005) {
    param = update;
  }
}

void UpdateEncoderPressed() {
  // switch modes
  if (pod.encoder.RisingEdge()) {
    edit = !edit;
    step = 0;
  }
  if (edit == false) {
    editCycle = false;
  }
}

void UpdateEncoderIncrement() {
  if (edit) {
    step += pod.encoder.Increment();
    step = (step % 8 + 8) % 8;
  } else {
    wave += pod.encoder.Increment();
    wave = ((wave % 2 + 2) % 2);
    osc.SetWaveform(wave + 3);
  }
}

void UpdateButtons() {
  if (edit) {
    if (pod.buttons[1].RisingEdge()) {
      active[step] = !active[step];
    }
    if (pod.buttons[0].RisingEdge()) {
      editCycle = !editCycle;
    }
  }
}

void UpdateKnobs() {
  k1 = analogRead(PIN_POD_POT_1) / 1023.f;
  k2 = analogRead(PIN_POD_POT_2) / 1023.f;

  if (edit) {
    ConditionalParameter(oldk1, k1, dec[step], k1 * .97f + .03f);

    int temp = (int)(k2 * 10);
    ConditionalParameter(oldk2, k2, pitch[step],
                         pent[temp % 5] * (temp / (int)5 + 1));
    osc.SetFreq(pitch[step]);
  }

  else {
    ConditionalParameter(oldk1, k1, tickFrequency, k1 * 9.f + 4.f);
    ConditionalParameter(oldk2, k2, filterFrequency, k2 * 9900.f + 100.f);

    tick.SetFreq(tickFrequency);
    flt.SetFreq(filterFrequency);
  }

  oldk1 = k1;
  oldk2 = k2;
}

void UpdateLedEdit() {
  int tmp = step + 1;
  pod.leds[0].Set(tmp & 1, tmp & 2, tmp & 4);

  if (active[step]) {
    pod.leds[1].Set(0, 1, 0);
  }

  else {
    pod.leds[1].Set(0, 0, 0);
  }
}

void UpdateLeds() {
  if (edit) {
    UpdateLedEdit();
  } else {
    pod.leds[1].Set(0, env_out, 0);
    pod.leds[0].Set(0, env_out, 0);
  }
}

void Controls() {
  pod.DebounceControls();

  UpdateEncoderPressed();

  UpdateEncoderIncrement();

  UpdateButtons();

  UpdateKnobs();

  UpdateLeds();
}

void NextSamples(float &sig) {
  env_out = env.Process();
  osc.SetAmp(env_out);
  sig = osc.Process();
  sig = flt.Process(sig);

  if (tick.Process() && !edit) {
    step++;
    step %= 8;
    if (active[step]) {
      env.Trigger();
    }
  }

  if (active[step]) {
    env.SetTime(ADENV_SEG_DECAY, dec[step]);
    osc.SetFreq(pitch[step]);
  }
  if (!env.IsRunning() && editCycle) {
    env.Trigger();
  }
}