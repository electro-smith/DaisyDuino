#include "DaisyDuino.h"

static DaisyHardware petal;

float hardClip(float in)
{
    in = in > 1.f ? 1.f : in;
    in = in < -1.f ? -1.f : in;
    return in;
}

float softClip(float in)
{
    if(in > 0)
        return 1 - expf(-in);
    return -1 + expf(in);
}

bool        bypassHard, bypassSoft;
static void AudioCallback(float **in, float **out, size_t size)
{
    petal.DebounceControls();

    //float Pregain = petal.knobs[2].Process() * 10 + 1.2;
    //float Gain    = petal.knobs[3].Process() * 100 + 1.2;
    //float drywet  = petal.knobs[4].Process();


    float Pregain = 0.f;
    float Gain = 0.f;
    float drywet = 0.f;
  

    bypassSoft = petal.switches[0].RisingEdge() ? !bypassSoft : bypassSoft;
    bypassHard = petal.switches[1].RisingEdge() ? !bypassHard : bypassHard;

    for(size_t i = 0; i < size; i++)
    {
        for(int chn = 0; chn < 2; chn++)
        {
            in[chn][i] *= Pregain;
            float wet = in[chn][i];

            if(!bypassSoft || !bypassHard)
            {
                wet *= Gain;
            }

            if(!bypassSoft)
            {
                wet = softClip(wet);
            }

            if(!bypassHard)
            {
                wet = hardClip(wet);
            }

            out[chn][i] = wet * drywet + in[chn][i] * (1 - drywet);
        }
    }
}

void setup()
{
    petal = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);

    //bypassHard = bypassSoft = false;

    // start callback
    //DAISY.begin(AudioCallback);
}
    
  void loop()
    {
        //LED stuff
        petal.SetFootswitchLed(0, !bypassSoft);
        petal.SetFootswitchLed(1, !bypassHard);

        for(int i = 0; i < 8; i++)
        {
            petal.SetRingLed(i, 1.f, 0.f, 0.f);
        }

        petal.UpdateLeds();

        delay(6);
    }
