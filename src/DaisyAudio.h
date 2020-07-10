#ifndef DSY_ARDUINO_AUDIO_H
#define DSY_ARDUINO_AUDIO_H

// ENABLE HAL SAI 
#define HAL_SAI_MODULE_ENABLED

#include <stdio.h>
#include "Arduino.h"
#include "DaisyDSP.h"

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


//TODO
// encoder increment
// updateanalogcontrols?
// PWM for leds would be nice


//just does the increment stuff
//the user will use the switch for the pressed themselves
class Encoder
{
    public:

    void Init()
    {
	inc_ = 0;
	a_ = b_ = 0xff;
    }
    
    void Process(uint8_t a_in, uint8_t b_in)
    {
	a_ = (a_ << 1) | (a_in);
	b_ = (b_ << 1) | (b_in);

	inc_ = 0;
	
	if((b_ == 0x7f) && (a_  == 0x00))
	    inc_ = 1;
	else if ((a_ == 0x7f) && (b_ == 0x00))
	    inc_ = -1;
    }

    int32_t Increment() { return inc_; }
    
    private:    
        uint8_t a_, b_;
	int32_t inc_;
};

class Switch
{
    public:
        void Init(float update_rate, bool invert)
	{
	    flip_ = invert;
	    time_per_update_ = 1.f / update_rate;
	    state_ = 0;
	    time_held_ = 0.f;
	}

	//debounces and processes input
	void Process(uint8_t in)
	{
	    state_ = (state_ << 1) | (flip_ ? !in : in);
	    
	    if (state_ == 0x7f || state_ == 0x80)
		time_held_ = 0;
	    
	    if (state_ == 0xff)
		time_held_ += time_per_update_;
	}
    
	bool RisingEdge() { return state_ == 0x7f; }
	
	bool FallingEdge() { return state_ == 0x80; }
	
	bool Pressed() { return state_ == 0xff; }
	
	float TimeHeldMs() { return Pressed() ? time_held_ * 1000.f : 0; }
	
    private:
	bool flip_;
	float time_per_update_, time_held_;
	uint8_t state_;
};

class Pod
{
public:
    int knobPins[2];
    int switchPins[2];
    int ledPins[2][3]; //2 leds, RGB pins
    int encClickPin;
    int encIncPins[2]; //A, B

    Switch switches[2];
    
    Switch encSwitch;
    Encoder enc;
    
    void Init(float control_update_rate)
    {
	knobPins[0] = 21;
	knobPins[1] = 15;
	
	switchPins[0] = 27;
	switchPins[1] = 28;
	
	ledPins[0][0] = 20;
	ledPins[0][1] = 19;
	ledPins[0][2] = 18;
	ledPins[1][0] = 17;
	ledPins[1][1] = 24;
	ledPins[1][2] = 23;
	
	encClickPin = 13;
	encIncPins[0] = 26; //A
	encIncPins[1] = 25; //B
	    
        switches[0].Init(control_update_rate, true);
	switches[1].Init(control_update_rate, true);

	encSwitch.Init(control_update_rate, true);
	enc.Init();
    }
};

class AudioClass
{
    public:
        AudioClass();

        // Initializes the audio for the given platform, and returns the number of channels.
        size_t init(DaisyAudioDevice device, DaisyAudioSampleRate sr);
        void begin(DaisyAudioCallback cb);
        void end();
        float get_samplerate();
	
    private:
        size_t _blocksize;
        DaisyAudioSampleRate _samplerate;
        DaisyAudioDevice _device;
	void InitPins(DaisyAudioDevice device);
};

extern AudioClass DAISY;

#endif
