#include "DaisyDuino.h"
#include <string>
#include <U8g2lib.h>

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI oled(U8G2_R0, 8, 10, 7, 9, 30);

#define NUM_VOICES 32
#define MAX_DELAY ((size_t)(10.0f * 48000.0f))

// Hardware
DaisyHardware hw;

// Synthesis
PolyPluck<NUM_VOICES> synth;
// 10 second delay line on the external SDRAM
DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS del;
ReverbSc verb;

// Persistent filtered Value for smooth delay time changes.
float smooth_time;

void AudioCallback(float **in, float **out, size_t size) {
  float sig, delsig;           // Mono Audio Vars
  float trig, nn, decay;       // Pluck Vars
  float deltime, delfb, kval;  // Delay Vars
  float dry, send, wetl, wetr; // Effects Vars
  float samplerate;

  // Assign Output Buffers
  float *out_left, *out_right;
  out_left = out[0];
  out_right = out[1];

  samplerate = DAISY.get_samplerate();
  hw.DebounceControls();
  
  // Handle Triggering the Plucks
  trig = 0.0f;
  if (hw.encoder.RisingEdge() || hw.gateIns[0].Trig())
    trig = 1.0f;

  // Set MIDI Note for new Pluck notes.
  nn = (1023.f - analogRead(PIN_PATCH_CTRL_1)) / 1023.f;
  nn = 24.0f + nn * 60.0f;
  nn = static_cast<int32_t>(nn); // Quantize to semitones

  // Read knobs for decay;
  decay = (1023.f - analogRead(PIN_PATCH_CTRL_2)) / 1023.f;
  decay = 0.5f + decay * 0.5f;
  synth.SetDecay(decay);

  // Get Delay Parameters from knobs.
  kval = (1023.f - analogRead(PIN_PATCH_CTRL_3)) / 1023.f;;
  deltime = (0.001f + (kval * kval) * 5.0f) * samplerate;
  delfb = (1023.f - analogRead(PIN_PATCH_CTRL_4)) / 1023.f;

  // Synthesis.
  for (size_t i = 0; i < size; i++) {
    // Smooth delaytime, and set.
    fonepole(smooth_time, deltime, 0.0005f);
    del.SetDelay(smooth_time);

    // Synthesize Plucks
    sig = synth.Process(trig, nn);

    //		// Handle Delay
    delsig = del.Read();
    del.Write(sig + (delsig * delfb));

    // Create Reverb Send
    dry = sig + delsig;
    send = dry * 0.6f;
    verb.Process(send, send, &wetl, &wetr);

    // Output
    out_left[i] = dry + wetl;
    out_right[i] = dry + wetr;
  }
}

void setup(){
  // Init everything.
  float samplerate;
  hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  oled.setFont(u8g2_font_t0_12_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();

/*
  // briefly display the module name
  oled.clearBuffer();
  std::string str = "Pluck Echo";
  char *cstr = &str[0];
  oled.drawStr(0, 8, cstr);
  oled.sendBuffer();
  //delay(1000);*/

  synth.Init(samplerate);

  del.Init();
  del.SetDelay(samplerate * 0.8f); // half second delay

  verb.Init(samplerate);
  verb.SetFeedback(0.85f);
  verb.SetLpFreq(2000.0f);

  DAISY.begin(AudioCallback);
}

void loop() {
  oled.clearBuffer();

  int ctrls[4] = {PIN_PATCH_CTRL_1, PIN_PATCH_CTRL_2, PIN_PATCH_CTRL_3, PIN_PATCH_CTRL_4};
  char cstr[15];
  for(int i = 0; i < 4; i++){
    //int val = 1023 - analogRead(ctrls[i]); //1023 - 0 -> 0 - 1023
    //sprintf(cstr, "ctrl %d %d", i + 1, val);
    sprintf(cstr, "ctrl");
    oled.drawStr(0, (i + 1) * 8, cstr);
  }
  
  oled.sendBuffer();
}