// Title: SynthVoice
// Description: Synth voice with adenv, vibrato, pitch and filter
// Hardware: Daisy Pod
// Author: Ben Sergentanis
// Controls:
// Encoder Press: Cycle waveforms
// Encoder: Control mode: 1,2,3
// Knob 1: Cutoff, Attack time, vibrato freq
// Knob 2: Pitch, Decay time, vibrato depth
// Button 1: Trigger envelope
// Button 2: Cycle envelope
// Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/pod/SynthVoice/resources/SynthVoice.png

#include "DaisyDuino.h"

DaisyHardware pod;
static Oscillator osc, lfo;
static MoogLadder flt;
static AdEnv ad;
static Parameter  pitchParam, cutoffParam, lfoParam;

int wave, mode;
float vibrato, oscFreq, lfoFreq, lfoAmp, attack, release, cutoff;
float oldk1, oldk2, k1, k2;
bool selfCycle;

void ConditionalParameter(float oldVal, float newVal, float &param,
                          float update);

void Controls();

void NextSamples(float &sig) {
  float ad_out = ad.Process();
  vibrato = lfo.Process();

  osc.SetFreq(oscFreq + vibrato);

  sig = osc.Process();
  sig = flt.Process(sig);
  sig *= ad_out;
}

void MyCallback(float **in, float **out, size_t size) {
  Controls();

  for (size_t i = 0; i < size; i++) {
    float sig;
    NextSamples(sig);

    out[0][i] = sig;
    out[1][i] = sig;
  }
}

void setup() {
    float sample_rate, callback_rate;

    pod = DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    sample_rate = DAISY.get_samplerate();
    callback_rate = DAISY.get_callbackrate();

    // Set global variables
    mode = 0;
    vibrato = 0.0f;
    oscFreq = 1000.0f;
    oldk1 = oldk2 = 0;
    k1 = k2 = 0;
    attack = .01f;
    release = .2f;
    cutoff = 10000;
    lfoAmp = 1.0f;
    lfoFreq = 0.1f;
    selfCycle = false;
    
    //Init everything
    osc.Init(sample_rate);
    flt.Init(sample_rate);
    ad.Init(sample_rate);
    lfo.Init(sample_rate);

    //Set filter parameters
    flt.SetFreq(10000);
    flt.SetRes(0.8);
    
    // Set parameters for oscillator
    osc.SetWaveform(osc.WAVE_SAW);
    wave = osc.WAVE_SAW;
    osc.SetFreq(440);
    osc.SetAmp(1);

    // Set parameters for lfo
    lfo.SetWaveform(osc.WAVE_SIN);
    lfo.SetFreq(0.1);
    lfo.SetAmp(1);
    
    //Set envelope parameters
    ad.SetTime( ADENV_SEG_ATTACK, 0.01);
    ad.SetTime( ADENV_SEG_DECAY, .2);
    ad.SetMax(1);
    ad.SetMin(0);
    ad.SetCurve(0.5);

    //set parameter parameters
    cutoffParam.Init(pod.controls[0], 100, 20000, cutoffParam.LOGARITHMIC);
    pitchParam.Init(pod.controls[1], 50, 3000, pitchParam.LOGARITHMIC);
    lfoParam.Init(pod.controls[0], 0.25, 1000, lfoParam.LOGARITHMIC);

    //start the audio callback
    DAISY.begin(MyCallback);
}

void loop() {}

// Updates values if knob had changed
void ConditionalParameter(float oldVal, float newVal, float &param,
                          float update) {
  if (abs(oldVal - newVal) > 0.0005) {
    param = update;
  }
}

// Controls Helpers
void UpdateEncoder() {
  wave += pod.encoder.RisingEdge();
  wave %= osc.WAVE_POLYBLEP_TRI;

  // skip ramp since it sounds like saw
  if (wave == 3) {
    wave = 4;
  }

  osc.SetWaveform(wave);

  mode += pod.encoder.Increment();
  mode = (mode % 3 + 3) % 3;
}

void UpdateKnobs()
{
    k1 = pod.controls[0].Value();
    k2 = pod.controls[1].Value();

    switch (mode)
    {
        case 0:
            ConditionalParameter(oldk1, k1, cutoff, cutoffParam.Process());
            ConditionalParameter(oldk2, k2, oscFreq, pitchParam.Process());
            flt.SetFreq(cutoff);
            break;
        case 1:
            ConditionalParameter(oldk1, k1, attack, k1 + .02);
            ConditionalParameter(oldk2, k2, release, k2 + .02);
            ad.SetTime(ADENV_SEG_ATTACK, attack);
            ad.SetTime(ADENV_SEG_DECAY, release);
            break;
        case 2:
            ConditionalParameter(oldk1, k1, lfoFreq, lfoParam.Process());
            ConditionalParameter(oldk2, k2, lfoAmp, k2 * 100);
            lfo.SetFreq(lfoFreq);
            lfo.SetAmp(lfoAmp);
        default:
            break;
    }

    oldk1 = k1;
    oldk2 = k2;    

}

void UpdateLeds() {
  pod.leds[0].Set(mode == 2, mode == 1, mode == 0);
  pod.leds[1].Set(false, selfCycle, selfCycle);

  oldk1 = k1;
  oldk2 = k2;
}

void UpdateButtons() {
  if (pod.buttons[0].RisingEdge() || (selfCycle && !ad.IsRunning())) {
    ad.Trigger();
  }

  if (pod.buttons[1].RisingEdge()) {
    selfCycle = !selfCycle;
  }
}

void Controls()
{
  pod.ProcessAllControls();

  UpdateEncoder();

  UpdateKnobs();

  UpdateLeds();

  UpdateButtons();
}