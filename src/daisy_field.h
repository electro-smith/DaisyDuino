#include "utility/led_driver.h"

#define PIN_FIELD_GATE_IN 0

#define PIN_FIELD_SPI_CS 7
#define PIN_FIELD_SPI_SCK 8
#define PIN_FIELD_OLED_CMD 9
#define PIN_FIELD_SPI_MOSI 10
#define PIN_FIELD_I2C_SCL 11
#define PIN_FIELD_I2C_SDA 12

#define PIN_FIELD_MIDI_OUT 13
#define PIN_FIELD_MIDI_IN 14

#define PIN_FIELD_GATE_OUT 15

#define PIN_FIELD_ADC_POT_MUX A1

#define PIN_FIELD_ADC_CV_1 A2
#define PIN_FIELD_ADC_CV_2 A3
#define PIN_FIELD_ADC_CV_4 A9 // Jumped on Rev2 from 22
#define PIN_FIELD_ADC_CV_3 A10 // Jumped on Rev2 from 23

#define PIN_FIELD_MUX_SEL_2 19
#define PIN_FIELD_MUX_SEL_1 20
#define PIN_FIELD_MUX_SEL_0 21

#define PIN_FIELD_CD4021_D1 26
#define PIN_FIELD_CD4021_CS 27
#define PIN_FIELD_CD4021_CLK 28

#define PIN_FIELD_SWITCH_2 29
#define PIN_FIELD_SWITCH_1 30

#define PIN_FIELD_DAC_2 22    // Jumped on Rev2 from 24
#define PIN_FIELD_DAC_1 23    // Jumped on Rev2 from 25

static LedDriverPca9685<2, true>::DmaBuffer DMA_BUFFER_MEM_SECTION
    field_led_dma_buffer_a,
    field_led_dma_buffer_b;

enum
{
    FIELD_LED_KEY_A1, 
    FIELD_LED_KEY_A2, 
    FIELD_LED_KEY_A3, 
    FIELD_LED_KEY_A4, 
    FIELD_LED_KEY_A5, 
    FIELD_LED_KEY_A6, 
    FIELD_LED_KEY_A7, 
    FIELD_LED_KEY_A8, 
    FIELD_LED_KEY_B8, 
    FIELD_LED_KEY_B7, 
    FIELD_LED_KEY_B6, 
    FIELD_LED_KEY_B5, 
    FIELD_LED_KEY_B4, 
    FIELD_LED_KEY_B3, 
    FIELD_LED_KEY_B2, 
    FIELD_LED_KEY_B1, 
    FIELD_LED_KNOB_1, 
    FIELD_LED_KNOB_2, 
    FIELD_LED_KNOB_3, 
    FIELD_LED_KNOB_4, 
    FIELD_LED_KNOB_5, 
    FIELD_LED_KNOB_6, 
    FIELD_LED_KNOB_7, 
    FIELD_LED_KNOB_8, 
    FIELD_LED_LAST    
};