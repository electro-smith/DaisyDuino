#ifndef DSY_ARDUINO_AUDIO_H
#define DSY_ARDUINO_AUDIO_H

// ENABLE HAL MODULES
#define HAL_SAI_MODULE_ENABLED

#include <stdio.h>
#include "Arduino.h"
#include "DaisyDSP.h"
#include "daisy_pod.h"
#include "daisy_patch.h"
#include "utility/audio.h"
#include "utility/sdram.h"

enum DaisyDuinoDevice {
    DAISY_SEED,
    DAISY_POD,
    DAISY_PETAL,
    DAISY_FIELD,
    DAISY_PATCH,
    DAISY_LAST,
};

enum DaisyDuinoSampleRate {
    AUDIO_SR_48K,
    AUDIO_SR_96K,
    AUDIO_SR_LAST,
};

typedef void (*DaisyDuinoCallback)(float**, float**, size_t);


//TODO
// updateanalogcontrols? (likely covered by analogRead)
// Parameter would be nice
// PWM for leds would be nice too

class Led
{
    public:
        void Init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b)
	{
            pin_r_ = pin_r;
	    pin_g_ = pin_g;
	    pin_b_ = pin_b;

	    pinMode(pin_r, OUTPUT);
	    pinMode(pin_g, OUTPUT);
	    pinMode(pin_b, OUTPUT);
	}

	//no PWM for now
        void Set(bool r, bool g, bool b)
	{
	    digitalWrite(pin_r_, !r);
	    digitalWrite(pin_g_, !g);
	    digitalWrite(pin_b_, !b);
	}
	
    private:
        uint8_t pin_r_, pin_g_, pin_b_;
};

class Switch
{
    public:
        void Init(float update_rate, bool invert, uint8_t pin, uint8_t mode)
	{
	    flip_ = invert;
	    time_per_update_ = 1.f / update_rate;
	    state_ = 0;
	    time_held_ = 0.f;
	    pin_ = pin;
	    pinMode(pin, mode);
	}

	//debounces and processes input
	void Debounce()
	{
	    uint8_t in = digitalRead(pin_);
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
	uint8_t state_, pin_;
};

class Encoder
{
    public:

    void Init(float update_rate, uint8_t pinA, uint8_t pinB, uint8_t pinClick, uint8_t modeA, uint8_t modeB, uint8_t modeC)
    {
	inc_ = 0;
	a_ = b_ = 0xff;

	pinA_ = pinA;
	pinB_ = pinB;

	pinMode(pinA, modeA);
	pinMode(pinB, modeB);
	
	encSwitch.Init(update_rate, true, pinClick, modeC);
    }
    
    void Debounce()
    {
	uint8_t a_in = digitalRead(pinA_);
	uint8_t b_in = digitalRead(pinB_);

	encSwitch.Debounce();
	
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
        
    bool RisingEdge()  { return encSwitch.RisingEdge(); }
	
    bool FallingEdge() { return encSwitch.FallingEdge(); }
    
    bool Pressed()     { return encSwitch.Pressed(); }
    
    float TimeHeldMs() { return encSwitch.TimeHeldMs(); }
	
    private:    
        Switch encSwitch;
        uint8_t a_, b_, pinA_, pinB_;
	int32_t inc_;
};

class GateIn
{
    public:
    void Init(uint8_t pin, uint8_t mode, bool invert)
    {
		sw.Init(1, invert, pin, mode);
    }

    bool State()
    {
		return sw.Pressed();
    }
    
    bool Trig()
    {
		return sw.RisingEdge();
	}
	
	void Debounce()
	{
		sw.Debounce();
	}

    private:
        Switch sw;
};

class DaisyHardware
{
public:
    Switch buttons[2];
    Encoder encoder;
    Led leds[2];
    GateIn gateIns[2];
    
    int num_channels;
    int numSwitches, numLeds, numGates;

    void Init(float control_update_rate, DaisyDuinoDevice device)
    {
	device_ = device;
	num_channels = 2;
	numGates = 0;
	
	switch (device){
	    case DAISY_SEED:
		break;
		
	    case DAISY_POD:
	        buttons[0].Init(control_update_rate, true, PIN_POD_SWITCH_1, INPUT_PULLUP);
	        buttons[1].Init(control_update_rate, true, PIN_POD_SWITCH_2, INPUT_PULLUP);
				
			encoder.Init(control_update_rate, PIN_POD_ENC_A, PIN_POD_ENC_B, PIN_POD_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);

			leds[0].Init(PIN_POD_LED_1_RED, PIN_POD_LED_1_GREEN, PIN_POD_LED_1_BLUE);    
	        leds[1].Init(PIN_POD_LED_2_RED, PIN_POD_LED_2_GREEN, PIN_POD_LED_2_BLUE);

			numSwitches = numLeds = 2;
			break;

	    case DAISY_PETAL:
		break;
	    
		case DAISY_FIELD:
		break;
	    
		case DAISY_PATCH:
			encoder.Init(control_update_rate, PIN_PATCH_ENC_A, PIN_PATCH_ENC_B, PIN_PATCH_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);
			gateIns[0].Init(20, INPUT, true);
			gateIns[1].Init(19, INPUT, true);
			numSwitches = numLeds = 0;
			numGates = 2;
			num_channels = 4;
		break;
	    default:
		break;
	}
    }

    void DebounceControls()
    {
	for (int i = 0; i < numSwitches; i++)
	{
	    buttons[i].Debounce();
	}
	
		encoder.Debounce();
		for (int i = 0; i < numGates; i++)
		{
			gateIns[i].Debounce();
		}
	}

private:
    DaisyDuinoDevice device_;
};

class AudioClass
{
    public:
        AudioClass();

        // Initializes the audio for the given platform, and returns a DaisyHardware object
		//default samplerate is 48kHz
        DaisyHardware init(DaisyDuinoDevice device, DaisyDuinoSampleRate sr = AUDIO_SR_48K);
        void begin(DaisyDuinoCallback cb);
        void end();
        float get_samplerate();
 	inline float get_callbackrate() { return get_samplerate() / _blocksize; }
	
	AudioHandle audio_handle;
	
    private:
        size_t _blocksize;
        DaisyDuinoSampleRate _samplerate;
        DaisyDuinoDevice _device;

        dsy_sdram_handle sdram_handle;
	void ConfigureSdram();
};

extern AudioClass DAISY;

#endif
