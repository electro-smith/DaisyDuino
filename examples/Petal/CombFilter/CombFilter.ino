#include "DaisyDuino.h"

DaisyHardware petal;
Comb comb;
Oscillator lfo;
CrossFade fader;

bool bypassOn;
float buf[9600];

Parameter lfoFreqParam, lfoAmpParam;
Parameter combFreqParam, combRevParam;
Parameter faderPosParam;

float targetCombFreq, combFreq;

void AudioCallback(float **in, float **out, size_t size) {
    UpdateControls();

  for (size_t i = 0; i < size; i++) {
    fonepole(combFreq, targetCombFreq, .0001f);
    float f = combFreq + lfo.Process() + 50.f;
    comb.SetFreq(f);

    float inf = in[0][i];
    float process = comb.Process(in[0][i]);
    out[0][i] = out[1][i] = fader.Process(inf, process);
  }
}

int ringlednum;
void setup() {
  float samplerate;
  petal = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);
  samplerate = DAISY.AudioSampleRate();

  lfoFreqParam.Init(petal.controls[0], 0, 2, Parameter::LINEAR);
  lfoAmpParam.Init(petal.controls[1], 0, 50, Parameter::LINEAR);
  combFreqParam.Init(petal.controls[2], 25, 300, Parameter::LOGARITHMIC);
  combRevParam.Init(petal.controls[3], 0, 1, Parameter::LINEAR);
  faderPosParam.Init(petal.controls[4], 0, 1, Parameter::LINEAR);

  lfo.Init(samplerate);
  lfo.SetAmp(1);
  lfo.SetWaveform(Oscillator::WAVE_SIN);

  for (int i = 0; i < 9600; i++) {
    buf[i] = 0.0f;
  }

  // initialize Comb object
  comb.Init(samplerate, buf, 9600);

  fader.Init();

  ringlednum = 0;
  bypassOn = false;

  DAISY.begin(AudioCallback);
}

void UpdateControls() {
  petal.ProcessDigitalControls();

  // knobs
  lfo.SetFreq(lfoFreqParam.Process());
  lfo.SetAmp(lfoAmpParam.Process());

  targetCombFreq = combFreqParam.Process();

  comb.SetRevTime(combRevParam.Process());

  fader.SetPos(faderPosParam.Process());
  if (bypassOn) {
    fader.SetPos(0);
  }

  // bypass switch
  if (petal.buttons[0].RisingEdge()) {
    bypassOn = !bypassOn;
  }
}

void UpdateLeds(){
  petal.ClearLeds();

  ringlednum = (ringlednum + 1) % 80;
  
  petal.SetFootswitchLed(0, !bypassOn);
  petal.SetRingLed(ringlednum / 10, 0, 1, 1);
  petal.UpdateLeds();
}

void loop() {
  UpdateLeds();
}