#include "AUDIO.h"

AudioClass AUDIO;

AudioClass::AudioClass() : _blocksize{48}, _samplerate{AUDIO_SR_48K};
{
    // Initializes the audio for the given platform, and returns the number of channels.
}

uint32_t init(DaisyAudioDevice device, DaisyAudioSampleRate sr)
{
    // Set Audio Device, num channels, etc.
    // Only difference is Daisy Patch has second AK4556 and 4 channels
    dsy_audio_init();
    return device < DAISY_PATCH ? 2 : 4;
}

void begin(DaisyAudioCallback cb)
{
    dsy_audio_start(cb);
}
void end()
{
    dsy_audio_stop();
}
