// Title: FilterBank
// Description: Fixed Resonant Filter Bank
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Diagram: https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/FilterBank/resources/FilterBank.png
// Controls
// Ctrls 1-4: Change the amplitude of the currently selected filter
// Encoder: Switch between 4 sets of filters to map to the controls
// The other filter will remember their last settings after you scroll away and keep working

#include <U8g2lib.h>
#include "DaisyDuino.h"

DaisyHardware patch;

//oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/ 8, /* data=*/ 10, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 30);  
int num_channels;

int freqs[16];
int bank;

struct ConditionalUpdate
{
    float oldVal = 0;

    bool Process(float newVal)
    {
        if (abs(newVal - oldVal) > .04)
  {
      oldVal = newVal;
      return true;
  }
  
  return false;
    }
};

ConditionalUpdate condUpdates[4];

struct Filter{
    Svf filt;
    float amp;
    
    void Init(float samplerate, float freq)
    {
        filt.Init(samplerate);
        filt.SetRes(1);
        filt.SetDrive(.002);
        filt.SetFreq(freq);
        amp = .5f;
    }
    
    float Process(float in)
    {
        filt.Process(in);
        return filt.Peak() * amp;
    }
};

Filter filters[16];
uint8_t controls[4];

static void AudioCallback(float **in, float **out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        float sig = 0.f;
        for (int j = 0; j < 16; j++)
        {
            sig += filters[j].Process(in[0][i]);
        }
        sig *= .06;
  
        out[0][i] = out[1][i] = out[2][i] = out[3][i] = sig;
    }
}

void InitFreqs()
{
    freqs[0] = 50;
    freqs[1] = 75;
    freqs[2] = 110;
    freqs[3] = 150;
    freqs[4] = 220;
    freqs[5] = 350;
    freqs[6] = 500;
    freqs[7] = 750;
    freqs[8] = 1100;
    freqs[9] = 1600;
    freqs[10] = 2200;
    freqs[11] = 3600;
    freqs[12] = 5200;
    freqs[13] = 7500;
    freqs[14] = 11000;
    freqs[15] = 15000;
}

void InitFilters(float samplerate)
{
    for (int i = 0; i < 16; i++)
    {
        filters[i].Init(samplerate, freqs[i]);
    }
}

void setup()
{
    float samplerate;
    patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K); 
    num_channels = patch.num_channels;
    samplerate = DAISY.get_samplerate();

    InitFreqs();
    InitFilters(samplerate);
    bank = 0;
   
    oled.setFont(u8x8_font_chroma48medium8_r);
    oled.begin();
    UpdateOled();
    
    DAISY.begin(AudioCallback);
}

void loop()
{
    UpdateControls();
}

void UpdateOled()
{
    oled.clear();

    oled.drawString(0, 0, "FilterBank");

    String str = "";
    for (int i = 0; i < 2; i++)
    {
        str += String(freqs[i + 4 * bank]);
        str += "  ";
    }

    char* cstr = &str[0];
    oled.drawString(0,2,cstr);

    str = "";
    for (int i = 2; i < 4; i++)
    {
        str += String(freqs[i + 4 * bank]);
        str += "  ";
    }
    cstr = &str[0];
    oled.drawString(0,4,cstr);
}

void UpdateControls()
{
    patch.ProcessAllControls();
    
    //encoder
    int inc = patch.encoder.Increment();
    bank += inc;
    bank = (bank % 4 + 4) % 4;

    bool edge = patch.encoder.RisingEdge();
    bank = edge ? 0 : bank;

    //controls
    for (int i = 0 ; i < 4; i++)
    {
        float val = patch.controls[i].Value();
        if (condUpdates[i].Process(val))
        {
            filters[i + bank * 4].amp = val;
        }
    }

    if (edge || inc != 0)
        UpdateOled();
}