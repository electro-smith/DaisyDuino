#include "codec_ak4556.h"
#include "system.h"
#include "gpio.h"

namespace daisy
{
void Ak4556::Init(dsy_gpio_pin reset_pin)
{
    dsy_gpio reset;
    reset.pin  = reset_pin;
    reset.mode = DSY_GPIO_MODE_OUTPUT_PP;
    reset.pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&reset);
    dsy_gpio_write(&reset, 1);
    System::Delay(1);
    dsy_gpio_write(&reset, 0);
    System::Delay(1);
    dsy_gpio_write(&reset, 1);
}

} // namespace daisy
