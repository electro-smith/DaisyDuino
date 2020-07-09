#include "DaisyAudio.h"

#define NUM_OUTS 4

float mtof(float m) { return pow(2, (m -69.0) / 12.0) * 440.0; }

class Sine
{
  public:
  void Init() { phs = 0.0; SetFreq(100);}
  void SetFreq(float freq) { phs_inc = (2.0 * PI * freq) * (1.0 / 48000.0); }
  float Process() { float out; out = sin(phs) * 0.5; phs = phs + phs_inc; if (phs > (2.0 * PI)) { phs -= 2.0 * PI; }; return out; }
  private:
  float phs, phs_inc;
};

size_t num_channels;
unsigned long knobval;
Sine osc[NUM_OUTS];
float maj_chord[NUM_OUTS] = {0, 4, 7, 11};

void SineCallback(float **in, float **out, size_t size)
{
    float root = 60;
    root = 36.0 + (((1023 - knobval) / 1023.0) * 60.0);
    for (size_t chn = 0; chn < NUM_OUTS; chn++)
    {
      osc[chn].SetFreq(mtof(root + maj_chord[chn]));
    }
    for (size_t i = 0; i < size; i++)
    {
      // Synthesize sine wave per channel
      for (size_t chn = 0; chn < NUM_OUTS; chn++)
      {
        out[chn][i] = osc[chn].Process();
      }
    }
}

void setup()
{
    num_channels = DAISY.init(DAISY_PATCH, AUDIO_SR_48K); 
    osc[0].Init();
    osc[1].Init();
    osc[2].Init();
    osc[3].Init();
    DAISY.begin(SineCallback);
    Serial.begin(9600);
}

void loop()
{
  knobval = analogRead(A0);
  Serial.println(knobval);
}
