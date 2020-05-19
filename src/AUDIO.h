#ifndef DSY_AUDIO_H
#define DSY_AUDIO_H
#include "Arduino.h"
#include "utility/audio_com.h"

enum DaisyAudioDevice {
    DAISY_SEED,
    DAISY_POD,
    DAISY_PETAL,
    DAISY_FIELD,
    DAISY_PATCH,
    DAISY_LAST,
};

enum DaisyAudioSampleRate {
    AUDIO_SR_48K,
    AUDIO_SR_96K,
    AUDIO_SR_LAST,
}


class AudioClass
{
    public:
        AudioClass();

        // Initializes the audio for the given platform, and returns the number of channels.
        uint32_t init(DaisyAudioDevice device, DaisyAudioSampleRate sr);
        void begin(DaisyAudioCallback cb);
        void end();

    private:
        uint32_t _blocksize;
        DaisyAudioSampleRate _samplerate;
}

extern AudioClass AUDIO;

#endif
