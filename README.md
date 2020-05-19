# ArduinoAudio
Arduino Audio library for Daisy platform.

Sets up Daisy hardware for Audio I/O.

## Features

* Circular DMA transfers and reception with user callback.
* Configuration of samplerate.
* flexible callback interface

## callback

The callback must be a function with these arguments:

```void AudioCallback(float **in, float **out, uint32_t size);```

where in and out are pointers to a 2-day array `buffer[num_channels][blocksize]`

size is the number of samples to be processed.

