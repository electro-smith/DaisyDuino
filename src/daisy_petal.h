#include "utility/led_driver.h"

#define PIN_PETAL_SWITCH_1 8
#define PIN_PETAL_SWITCH_2 9
#define PIN_PETAL_SWITCH_3 10
#define PIN_PETAL_SWITCH_4 13
#define PIN_PETAL_SWITCH_5 25
#define PIN_PETAL_SWITCH_6 26
#define PIN_PETAL_SWITCH_7 7

#define PIN_PETAL_ENC_A 28
#define PIN_PETAL_ENC_B 27
#define PIN_PETAL_ENC_CLICK 14

#define PIN_PETAL_EXPRESSION 15
#define PIN_PETAL_POT_1 16
#define PIN_PETAL_POT_2 19
#define PIN_PETAL_POT_3 17
#define PIN_PETAL_POT_4 20
#define PIN_PETAL_POT_5 18
#define PIN_PETAL_POT_6 21

//2x pca9685 led driver
#define PIN_PETAL_I2C1_SDA 12
#define PIN_PETAL_I2C1_SCL 11

static LedDriverPca9685<2, true>::DmaBuffer DMA_BUFFER_MEM_SECTION
  petal_led_dma_buffer_a,
  petal_led_dma_buffer_b;

enum LedOrder
{
    LED_RING_1_R,
    LED_RING_1_G,
    LED_RING_1_B,
    LED_RING_5_R,
    LED_RING_5_G,
    LED_RING_5_B,
    LED_RING_2_R,
    LED_RING_2_G,
    LED_RING_2_B,
    LED_RING_6_R,
    LED_RING_6_G,
    LED_RING_6_B,
    LED_RING_3_R,
    LED_RING_3_G,
    LED_RING_3_B,
    LED_FS_1,
    LED_RING_4_R,
    LED_RING_4_G,
    LED_RING_4_B,
    LED_RING_7_R,
    LED_RING_7_G,
    LED_RING_7_B,
    LED_RING_8_R,
    LED_RING_8_G,
    LED_RING_8_B,
    LED_FS_2,
    LED_FS_3,
    LED_FS_4,
    LED_FAKE1,
    LED_FAKE2,
    LED_FAKE3,
    LED_FAKE4,
    LED_LAST,
};

