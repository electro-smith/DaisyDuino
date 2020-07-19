#include "DaisyDuino.h"

uint8_t led_pin;
uint8_t knob_pin;

void setup()
{    
    led_pin = 28;
    knob_pin = 21;

    //We'll plug our led into pin 28.
    pinMode(led_pin, OUTPUT);
}

void loop(){
    // The led brightness will be a value 0 - 1023
    unsigned int brightness = analogRead(knob_pin);

    //simple PWM led brightness
    for (int i = 0; i < brightness; i++)
    {
        digitalWrite(led_pin, HIGH);
    }
    
    for (int i = 0; i < 1023 - brightness; i++)
    {
        digitalWrite(led_pin, LOW);
    }
}