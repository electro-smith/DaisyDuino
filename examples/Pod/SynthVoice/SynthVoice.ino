#include "DaisyAudio.h"

DaisyPod pod;
static Oscillator osc, lfo;
static MoogLadder flt;
static AdEnv ad;

int wave, mode;
float vibrato, oscFreq, lfoFreq, lfoAmp, attack, release, cutoff;
float oldk1, oldk2, k1, k2;
bool selfCycle;

void ConditionalParameter(float oldVal, float newVal, float &param, float update);

void Controls();

void NextSamples(float& sig)
{
    float ad_out = ad.Process();
    vibrato = lfo.Process();
    
    osc.SetFreq(oscFreq + vibrato);
    
    sig = osc.Process();
    sig = flt.Process(sig);
    sig *= ad_out;
}

void MyCallback(float **in, float **out, size_t size)
{
    Controls();

    for (size_t i = 0; i < size; i++)
    {
        float sig;
        NextSamples(sig);

        out[0][i] = sig;
        out[1][i] = sig;
    }
}

void setup() {
    float sample_rate, callback_rate;

    DAISY.init(DAISY_POD, AUDIO_SR_48K); 
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
    pod.Init(callback_rate);
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
//   cutoffParam.Init(pod.knob1, 100, 20000, cutoffParam.LOGARITHMIC);
//    pitchParam.Init(pod.knob2, 50, 5000, pitchParam.LOGARITHMIC);
//    lfoParam.Init(pod.knob1, 0.25, 1000, lfoParam.LOGARITHMIC);

    //led pins
    pinMode(PIN_POD_LED_1_RED, OUTPUT);
    pinMode(PIN_POD_LED_1_GREEN, OUTPUT);
    pinMode(PIN_POD_LED_1_BLUE, OUTPUT);

    pinMode(PIN_POD_LED_2_RED, OUTPUT);
    pinMode(PIN_POD_LED_2_GREEN, OUTPUT);
    pinMode(PIN_POD_LED_2_BLUE, OUTPUT);

    //start the audio callback
    DAISY.begin(MyCallback);
}

void loop() {}

//Updates values if knob had changed
void ConditionalParameter(float oldVal, float newVal, float &param, float update)
{
    if (abs(oldVal - newVal) > 0.0005)
    {
        param = update;
    }
}


//Controls Helpers
void UpdateEncoder()
{
    pod.Debounce();

    wave += pod.encoder.RisingEdge();
    wave %= osc.WAVE_POLYBLEP_TRI;

    //skip ramp since it sounds like saw
    if (wave == 3)
    {  
        wave = 4;
    }

    osc.SetWaveform(wave);
    
    mode += pod.encoder.Increment();
    mode = (mode % 3 + 3) % 3;
}

void UpdateKnobs()
{
    k1 = (float)analogRead(PIN_POD_POT_1) / 1023.f; //10 bit reading
    k2 = (float)analogRead(PIN_POD_POT_2) / 1023.f;

    switch (mode)
    {
        case 0:
            ConditionalParameter(oldk1, k1, cutoff, k1 * 20000);
            ConditionalParameter(oldk2, k2, oscFreq, k2 * 5000);
            flt.SetFreq(cutoff);
            break;
        case 1:
            ConditionalParameter(oldk1, k1, attack, k1);
            ConditionalParameter(oldk2, k2, release, k2);
            ad.SetTime(ADENV_SEG_ATTACK, attack);
            ad.SetTime(ADENV_SEG_DECAY, release);
            break;
        case 2:
            ConditionalParameter(oldk1, k1, lfoFreq, k1 * 1000 + .25);
            ConditionalParameter(oldk2, k2, lfoAmp, k2 * 100);
            lfo.SetFreq(lfoFreq);
            lfo.SetAmp(lfoAmp);
        default:
            break;
    }
}

void UpdateLeds()
{
    //Pod LEDS are active low, so these are all backwards
    digitalWrite(PIN_POD_LED_1_RED, mode != 2);
    digitalWrite(PIN_POD_LED_1_GREEN, mode != 1);
    digitalWrite(PIN_POD_LED_1_BLUE, mode != 0);

    digitalWrite(PIN_POD_LED_2_RED, HIGH);
    digitalWrite(PIN_POD_LED_2_GREEN, !selfCycle);
    digitalWrite(PIN_POD_LED_2_BLUE, !selfCycle);    

    oldk1 = k1;
    oldk2 = k2;    
}

void UpdateButtons()
{
    pod.button1.Process(digitalRead(PIN_POD_SWITCH_1));
    pod.button2.Process(digitalRead(PIN_POD_SWITCH_2));

    if (pod.button1.RisingEdge() || (selfCycle && !ad.IsRunning()))
    {
        ad.Trigger();
    }

    if (pod.button2.RisingEdge())
    {
        selfCycle = !selfCycle;
    }
}

void Controls()
{
    UpdateEncoder();

    UpdateKnobs();

    UpdateLeds();

    UpdateButtons();
}
