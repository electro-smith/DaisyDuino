#include <MIDI.h>
#include "DaisyDuino.h"

DaisyHardware hw;

Oscillator osc;
AdEnv env;

MIDI_CREATE_DEFAULT_INSTANCE();

static void AudioCallback(float **in, float **out, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        out[0][i] = out[1][i] = osc.Process();
    }
}

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    env.Trigger();
    osc.SetFreq(60 * ((float)inNote / 12.f));
}

void setup()
{
    hw = DAISY.init(DAISY_POD, AUDIO_SR_48K); 
    float samplerate = DAISY.get_samplerate();

    osc.Init(samplerate);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetFreq(440);

    env.Init(samplerate);
    env.SetTime(ADENV_SEG_ATTACK, .1);
    env.SetTime(ADENV_SEG_DECAY, .5);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(0);

    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages

    DAISY.begin(AudioCallback);
}

void loop()
{
    // Read incoming messages
    MIDI.read();
}