#ifndef DSY_ARDUINO_AUDIO_H
#define DSY_ARDUINO_AUDIO_H

// ENABLE HAL SAI 
#define HAL_SAI_MODULE_ENABLED

#include <stdio.h>
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

typedef void (*DaisyAudioCallback)(float**, float**, size_t);


class AudioClass
{
    public:
        AudioClass();

        // Initializes the audio for the given platform, and returns the number of channels.
        size_t init(DaisyAudioDevice device, DaisyAudioSampleRate sr);
        void begin(DaisyAudioCallback cb);
        void end();

    private:
        size_t _blocksize;
        DaisyAudioSampleRate _samplerate;
        DaisyAudioDevice _device;
};

extern AudioClass AUDIO;

#endif
