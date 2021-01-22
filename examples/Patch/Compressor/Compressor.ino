
#include "DaisyDuino.h"
#include <string>
#include <U8g2lib.h>
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);


DaisyHardware patch;
Compressor comp;

bool isSideChained;

void UpdateControls();

static void AudioCallback(float **in, float **out, size_t size)
{
  float sig;
  float dry_in, dry_sidechain;
  UpdateControls();

  // Scales input by 2 and then the output by 0.5
  // This is because there are 6dB of headroom on the daisy
  // and currently no way to tell where '0dB' is supposed to be
  for (size_t i = 0; i < size; i++)
  {
    dry_in        = in[0][i] * 2.0f;
    dry_sidechain = in[1][i] * 2.0f;

    sig = isSideChained ? comp.Process(dry_in, dry_sidechain)
          : comp.Process(dry_in);

    // Writes output to all four outputs.
    for (size_t chn = 0; chn < 4; chn++)
    {
      out[chn][i] = sig * 0.5f;
    }
  }
}

void setup()
{
  float samplerate;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K); // Initialize hardware
  samplerate = DAISY.get_samplerate();

  comp.Init(samplerate);

  isSideChained = false;

  oled.setFont(u8g2_font_inb16_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();
  
  DAISY.begin(AudioCallback);
}

void loop()
{   
    //update the oled
    oled.clearBuffer();

    std::string str  = "Compressor";
    char *      cstr = &str[0];
    oled.drawStr(0, 0, cstr);

    str = "Sidechain: ";
    str += isSideChained ? "On" : "Off";
    oled.drawStr(0, 25, cstr);

    oled.sendBuffer();
}

void UpdateControls()
{
  patch.DebounceControls();

  //encoder click
  isSideChained = patch.encoder.RisingEdge() ? !isSideChained : isSideChained;

  //controls
  comp.SetThreshold(-80.f + 80.f * analogRead(PIN_PATCH_CTRL_1) / 1023.f); //-80 to 0
  comp.SetRatio(1.2f + 38.8f * analogRead(PIN_PATCH_CTRL_2) / 1023.f); //1.2 to 40
  comp.SetAttack(.01f + .99 * analogRead(PIN_PATCH_CTRL_3) / 1023.f); //.01 to .99  
  comp.SetRelease(.01f + .99 * analogRead(PIN_PATCH_CTRL_4) / 1023.f); //.01 to .99
}