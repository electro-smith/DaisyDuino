#ifndef DSY_ARDUINO_AUDIO_H
#define DSY_ARDUINO_AUDIO_H

// ENABLE HAL SAI 
#define HAL_SAI_MODULE_ENABLED

#include <stdio.h>
#include "Arduino.h"
#include "DaisyDSP.h"
#include "daisy_pod.h"

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
// updateanalogcontrols? (likely covered by analogRead)
// Parameter would be nice
// PWM for leds would be nice too

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

class Encoder
{
    public:

    void Init(float update_rate)
    {
	inc_ = 0;
	a_ = b_ = 0xff;

	encSwitch.Init(update_rate, true);
    }
    
    void ProcessInc(uint8_t a_in, uint8_t b_in)
    {
	a_ = (a_ << 1) | (a_in);
	b_ = (b_ << 1) | (b_in);

	inc_ = 0;
	
	if((a_ & 0x0f) == 0x0e && (b_ & 0x07) == 0x00)
	{
	    inc_ = 1;
	}
	else if((b_ & 0x0f) == 0x0e && (a_ & 0x07) == 0x00)
	{
	    inc_ = -1;
	}
    }

    int32_t Increment() { return inc_; }
    
    void ProcessClick(uint8_t in) { return encSwitch.Process(in); }
    
    bool RisingEdge()  { return encSwitch.RisingEdge(); }
	
    bool FallingEdge() { return encSwitch.FallingEdge(); }
    
    bool Pressed()     { return encSwitch.Pressed(); }
    
    float TimeHeldMs() { return encSwitch.TimeHeldMs(); }
	
    private:    
        Switch encSwitch;
        uint8_t a_, b_;
	int32_t inc_;
};

class Pod
{
public:
    Switch button1;
    Switch button2;
    
    Encoder encoder;
    
    void Init(float control_update_rate)
    {
        button1.Init(control_update_rate, true);
	button2.Init(control_update_rate, true);

	encoder.Init(control_update_rate);
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
 	inline float get_callbackrate() { return get_samplerate() / _blocksize; }
	
    private:
        size_t _blocksize;
        DaisyAudioSampleRate _samplerate;
        DaisyAudioDevice _device;
	void InitPins(DaisyAudioDevice device);
};

extern AudioClass DAISY;

#endif
