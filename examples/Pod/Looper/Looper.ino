#include "DaisyAudio.h"

#define MAX_SIZE (48000 * 60 * 5) // 5 minutes of floats at 48 khz

DaisyPod pod;

bool first = true;  //first loop (sets length)
bool rec   = false; //currently recording
bool play  = false; //currently playing

int   pos = 0;
float DSY_SDRAM_BSS buf[MAX_SIZE];
int                 mod = MAX_SIZE;
int                 len = 0;
float drywet = 0;
bool res = false;

void ResetBuffer();
void Controls();

void NextSamples(float &output, float* in, size_t i);

static void AudioCallback(float **in, float **out, size_t size)
{
    float output = 0;

    Controls();

    for(size_t i = 0; i < size; i++)
    {
        output = NextSamples(in, i);   

        // left and right outs
        out[0][i] = out[1][i] = output;
    }
}

void setup()
{
    float callback_rate;
    DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    callback_rate = DAISY.get_callbackrate();

    pod.Init(callback_rate);
    ResetBuffer();

    // start callback
    DAISY.begin(AudioCallback);
}

void loop()
{
}

//Resets the buffer
void ResetBuffer()
{
    play  = false;
    rec   = false;
    first = true;
    pos   = 0;
    len   = 0;
    for(int i = 0; i < mod; i++)
    {
        buf[i] = 0;
    }
    mod   = MAX_SIZE;
}

void UpdateButtons()
{
    //button1 pressed
    if(pod.button1.RisingEdge())
    {
        if(first && rec)
        {
            first = false;
            mod   = len;
            len   = 0;
        }

        res = true;
        play = true;
        rec  = !rec;
    }

    //button1 held
    if(pod.button1.TimeHeldMs() >= 1000 && res)
    {
        ResetBuffer();
        res = false;
    }
    
    //button2 pressed and not empty buffer
    if(pod.button2.RisingEdge() && !(!rec && first))
    {
        play = !play;
        rec = false;
    }
}

//Deals with analog controls 
void Controls()
{
    pod.DebounceControls();

    drywet = (float)analogRead(PIN_POD_POT_1) / 1023.f;

    UpdateButtons();

    //leds
    pod.led1.Set(rec, false, false);
    pod.led2.Set(false, play, false);
}

void WriteBuffer(float** in, size_t i)
{
    buf[pos] = buf[pos] * 0.5 + in[0][i] * 0.5;
    if(first)
    {
        len++;
    }
}

float NextSamples(float** in, size_t i)
{
    if (rec)
    {
        WriteBuffer(in, i);
    }
    
    float output = buf[pos];
    
    //automatic looptime
    if(len >= MAX_SIZE)
    {
        first = false;
        mod   = MAX_SIZE;
        len   = 0;
    }
    
    if(play)
    {
        pos++;
        pos %= mod;
    }

    if (!rec)
    {
        output = output * drywet + in[0][i] * (1 -drywet);
    }

    return output;
}
