// Title: Midi
// Description: TRS Midi Input. Triggers envelope and changes pitch on incoming
// MIDI notes Hardware: Daisy Patch Author: Ben Sergentanis Diagram:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/patch/Midi/resources/Midi.png

#include "DaisyDuino.h"
#include <MIDI.h>
#include <U8g2lib.h>

DaisyHardware patch;

Oscillator osc;
AdEnv env;

MIDI_CREATE_DEFAULT_INSTANCE();

// oled
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI oled(/* clock=*/8, /* data=*/10,
                                           /* cs=*/7, /* dc=*/9, /* reset=*/30);
int num_waves, num_channels;

static void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // Assign Synthesized Waveform to all four outputs.
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[0][i] = out[1][i] = out[2][i] = out[3][i] =
          osc.Process() * env.Process();
    }
  }
}

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity) {
  env.Trigger();
  osc.SetFreq(mtof(inNote));
}

void setup() {
  float samplerate;
  num_waves = Oscillator::WAVE_LAST - 1;
  patch = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
  num_channels = patch.num_channels;
  samplerate = DAISY.get_samplerate();

  osc.Init(samplerate); // Init oscillator

  env.Init(samplerate);
  env.SetTime(ADENV_SEG_ATTACK, .1);
  env.SetTime(ADENV_SEG_DECAY, .5);
  env.SetMax(1);
  env.SetMin(0);
  env.SetCurve(0);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Listen to all incoming messages

  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.begin();
  oled.drawString(6, 4, "Midi");

  DAISY.begin(AudioCallback);
}

void loop() {
  // Read incoming messages
  MIDI.read();
}
