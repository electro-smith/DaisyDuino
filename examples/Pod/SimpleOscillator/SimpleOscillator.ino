#include "DaisyAudio.h"

#define NUM_WAVEFORMS 4

Pod   hw;
Oscillator osc;

uint8_t waveforms[NUM_WAVEFORMS] = {
    Oscillator::WAVE_SIN,
    Oscillator::WAVE_TRI,
    Oscillator::WAVE_POLYBLEP_SAW,
    Oscillator::WAVE_POLYBLEP_SQUARE,
};

static float   freq;
float          sig;
static int     waveform, octave;

static void AudioCallback(float **in, float **out, size_t size)
{
    hw.encoder.ProcessInc(digitalRead(PIN_POD_ENC_A), digitalRead(PIN_POD_ENC_B));
    hw.encoder.ProcessClick(digitalRead(PIN_POD_ENC_CLICK));
    hw.button1.Process(digitalRead(PIN_POD_SWITCH_1));
    hw.button2.Process(digitalRead(PIN_POD_SWITCH_2));

    waveform += hw.encoder.Increment();
    waveform = DSY_CLAMP(waveform, 0, NUM_WAVEFORMS);
    osc.SetWaveform(waveforms[waveform]);

    if(hw.button2.RisingEdge())
        octave++;
    if(hw.button1.RisingEdge())
        octave--;
    octave = DSY_CLAMP(octave, 0, 4);

    // convert MIDI to frequency and multiply by octave size
    freq = analogRead(PIN_POD_POT_1) / 1023.f;
    freq = mtof(freq * 127 + (octave * 12));
    osc.SetFreq(freq);

    // Audio Loop
    for(size_t i = 0; i < size; i ++)
    {
        // Process
        sig        = osc.Process();
        out[0][i] = sig;
        out[1][i] = sig;
    }
}

void InitSynth(float samplerate)
{
    osc.Init(samplerate);
    osc.SetAmp(1.f);

    waveform = 0;
    octave   = 0;
}

void setup()
{
    float samplerate, callback_rate;
    DAISY.init(DAISY_POD, AUDIO_SR_48K);
    samplerate = DAISY.get_samplerate();
    callback_rate = DAISY.get_callbackrate();

    hw.Init(callback_rate);
    InitSynth(samplerate);

    //setup pins
    pinMode(PIN_POD_ENC_CLICK, INPUT_PULLUP);
    pinMode(PIN_POD_ENC_A, INPUT_PULLUP);
    pinMode(PIN_POD_ENC_B, INPUT_PULLUP); 

    pinMode(PIN_POD_SWITCH_1, INPUT_PULLUP);
    pinMode(PIN_POD_SWITCH_2, INPUT_PULLUP); 

    DAISY.begin(AudioCallback);
}

void loop()
{
}