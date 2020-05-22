# ArduinoAudio

Adds support for Arduino on the Daisy audio platform. This environment consists of an audio config library, DSP library, and board support using stm32duino.  

## Getting Started

The audio library can be installed just like any other Arduino library. Check out this tutorial for instructions. 
https://www.arduino.cc/en/guide/libraries

To add the Daisy board to your Arduino IDE, follow these instructions from stm32duino.
https://github.com/stm32duino/wiki/wiki/Getting-Started

## Running the examples

Running the example programs works the same way that all Arduino examples work. Click file->examples, and then scroll all the way to the bottom to until you see the "Audio" tab. You can then select one of our audio specific example programs. 

## Built With

* [Arduino](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [stm32duino](https://github.com/stm32duino/Arduino_Core_STM32) - Arduino Support for STM32 MCUs
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc



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

