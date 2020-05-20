#ifndef DSY_AUDIO_H
#define DSY_AUDIO_H

// ENABLE HAL SAI 
#define HAL_SAI_MODULE_ENABLED

#include "Arduino.h"

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
};


class AudioClass
{
    public:
        AudioClass();

        // Initializes the audio for the given platform, and returns the number of channels.
        unsigned long init(DaisyAudioDevice device, DaisyAudioSampleRate sr);
        void begin(DaisyAudioCallback cb);
        void end();

    private:
        unsigned long _blocksize;
        DaisyAudioSampleRate _samplerate;
};

extern AudioClass AUDIO;

#endif
