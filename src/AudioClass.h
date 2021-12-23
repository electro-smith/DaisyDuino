#ifndef DSY_ARDUINO_AUDIO_H
#define DSY_ARDUINO_AUDIO_H

#include "utility/audio.h"
#include "utility/sai.h"
#include "utility/sdram.h"
#include "utility/codec_pcm3060.h"
#include <stdio.h>

#include <Wire.h>

using namespace daisy;

enum DaisyDuinoSampleRate {
	AUDIO_SR_8K,
    AUDIO_SR_16K,
    AUDIO_SR_32K,
    AUDIO_SR_48K,
    AUDIO_SR_96K,
    AUDIO_SR_LAST,
};

class DaisyHardware; //forward declarations
enum DaisyDuinoDevice : short;

class AudioClass
{
    public:
        AudioClass() {}

        // Initializes the audio for the given platform, and returns a DaisyHardware object
		//default samplerate is 48kHz
        DaisyHardware init(DaisyDuinoDevice device, DaisyDuinoSampleRate sr = AUDIO_SR_48K);
        
		/** for bwd compatibility */
		void begin(AudioHandle::AudioCallback cb);

		/** for bwd compatibility. Does not work with Daisy Patch!! */
		void begin(AudioHandle::InterleavingAudioCallback cb);

		/** for bwd compatibility */			
		void end();

		/** for bwd compatibility */			
		float get_samplerate();
			
		/** for bwd compatibility */			
		float get_callbackrate();
		
		/** for bwd compatibility */			
		inline float get_blocksize();
		
		/** Interleaving callback not supported on patch. */
		void StartAudio(AudioHandle::InterleavingAudioCallback cb);

		void StartAudio(AudioHandle::AudioCallback cb);

		/** Won't work on Patch! */
		void ChangeAudioCallback(AudioHandle::InterleavingAudioCallback cb);

		void ChangeAudioCallback(AudioHandle::AudioCallback cb);

		void StopAudio();

		/** Works with the new samplerates */
		void SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate);

		float AudioSampleRate();

		void SetAudioBlockSize(size_t blocksize);

		size_t AudioBlockSize();

		float AudioCallbackRate();
				
    private:
		float callback_rate_;
        
		AudioHandle audio_handle;
		DaisyDuinoDevice _device;
        dsy_sdram_handle sdram_handle;
		
		/** Internal indices for DaisySeed-equivalent devices 
			*  This shouldn't have any effect on user-facing code,
			*  and only needs to be checked to properly initialize
			*  the onboard-circuits.
		*/
		enum class BoardVersion
		{
			/** Daisy Seed Rev4
				*  This is the original Daisy Seed */
			DAISY_SEED,
			/** Daisy Seed 1.1 (aka Daisy Seed Rev5)
				*  This is a pin-compatible version of the Daisy Seed
				*  that uses the WM8731 codec instead of the AK4430 */
			DAISY_SEED_1_1,
		};

		BoardVersion BoardVersionCheck();

	void ConfigureSdram();
};

extern AudioClass DAISY;

#endif