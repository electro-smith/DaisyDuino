#include "AUDIO.h"

AudioClass AUDIO;

AudioClass::AudioClass() : _blocksize{48}, _samplerate{AUDIO_SR_48K}
{
    // Initializes the audio for the given platform, and returns the number of channels.
}

unsigned long AudioClass::init(DaisyAudioDevice device, DaisyAudioSampleRate sr)
{
    // Set Audio Device, num channels, etc.
    // Only difference is Daisy Patch has second AK4556 and 4 channels
    _samplerate = sr;
    dsy_audio_init();
    return device < DAISY_PATCH ? 2 : 4;
}

void AudioClass::begin(DaisyAudioCallback cb)
{
    dsy_audio_start(cb);
}
void AudioClass::end()
{
    dsy_audio_stop();
}
