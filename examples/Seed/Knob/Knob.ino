#include "DaisyDuino.h"

size_t num_channels;

DaisyHardware hw;

uint8_t led_pin;
uint8_t knob_pin;

void setup()
{    
    float sample_rate;
    // Initialize for Daisy seed at 48kHz
    hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    num_channels = hw.num_channels;
    sample_rate = DAISY.get_samplerate();

    led_pin = 28;
    knob_pin = 21;

    //We'll plug our led into pin 28.
    pinMode(led_pin, OUTPUT);
}

void loop(){
    // The led brightness will be a value 0 - 1023
    unsigned int brightness = analogRead(knob_pin);

    for (int i = 0; i < brightness; i++)
    {
        digitalWrite(led_pin, HIGH);
    }
    for (int i = 0; i < 1023 - brightness; i++)
    {
        digitalWrite(led_pin, LOW);
    }
}