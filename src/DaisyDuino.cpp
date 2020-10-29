#include "DaisyDuino.h"

#include "utility/hid_audio.h"
#include "utility/sys_dma.h"
#include "utility/per_gpio.h"
#include "utility/sys_mpu.h"

#define PIN_AK4556_RESET 14
#define PORT_AK4556_RESET DSY_GPIOB

dsy_audio_handle haudio;
dsy_sai_handle hsai;

dsy_gpio ak4556_reset_pin;

AudioClass DAISY;

AudioClass::AudioClass() : _blocksize{48}, _samplerate{AUDIO_SR_48K}
{
    // Initializes the audio for the given platform, and returns the number of channels.
}

void AudioClass::ConfigureSdram()
{
    dsy_gpio_pin *pin_group;
    sdram_handle.state             = DSY_SDRAM_STATE_ENABLE;
    pin_group                      = sdram_handle.pin_config;
    pin_group[DSY_SDRAM_PIN_SDNWE] = dsy_pin(DSY_GPIOH, 5);

    dsy_sdram_init(&sdram_handle);
}

DaisyHardware AudioClass::init(DaisyDuinoDevice device, DaisyDuinoSampleRate sr)
{
    // Set Audio Device, num channels, etc.
    // Only difference is Daisy Patch has second AK4556 and 4 channels
    HAL_Init();
    SCB_DisableDCache(); // Still needs to wait for linker..
    _samplerate = sr;
    _device = device;
    _blocksize = 48; // default
    haudio.sai = &hsai;
    haudio.block_size = _blocksize;
    dsy_dma_init(); // may interfere with core STM32 Arduino stuff...
    dsy_mpu_init();
    
    // Set up audio
    // SAI1
    dsy_gpio_pin *pin_group;
    hsai.init = device == DAISY_PATCH ? DSY_AUDIO_INIT_BOTH : DSY_AUDIO_INIT_SAI1;
    hsai.device[DSY_SAI_1] = DSY_AUDIO_DEVICE_AK4556;
    hsai.samplerate[DSY_SAI_1] = DSY_AUDIO_SAMPLERATE_48K;
    hsai.bitdepth[DSY_SAI_1] = DSY_AUDIO_BITDEPTH_24;
    hsai.a_direction[DSY_SAI_1] = DSY_AUDIO_TX;
    hsai.b_direction[DSY_SAI_1] = DSY_AUDIO_RX;
    hsai.sync_config[DSY_SAI_1] = DSY_AUDIO_SYNC_MASTER;
    pin_group = hsai.sai1_pin_config;
    pin_group[DSY_SAI_PIN_MCLK] = dsy_pin(DSY_GPIOE, 2);
    pin_group[DSY_SAI_PIN_FS] = dsy_pin(DSY_GPIOE, 4);
    pin_group[DSY_SAI_PIN_SCK] = dsy_pin(DSY_GPIOE, 5);
    pin_group[DSY_SAI_PIN_SIN] = dsy_pin(DSY_GPIOE, 6);
    pin_group[DSY_SAI_PIN_SOUT] = dsy_pin(DSY_GPIOE, 3);
	// SAI2
    if (_device == DAISY_PATCH)
    {
        pin_group = hsai.sai2_pin_config;
        pin_group[DSY_SAI_PIN_MCLK] = dsy_pin(DSY_GPIOA, 1);
        pin_group[DSY_SAI_PIN_FS]   = dsy_pin(DSY_GPIOG, 9);
        pin_group[DSY_SAI_PIN_SCK]  = dsy_pin(DSY_GPIOA, 2);
        pin_group[DSY_SAI_PIN_SIN]  = dsy_pin(DSY_GPIOD, 11);
        pin_group[DSY_SAI_PIN_SOUT] = dsy_pin(DSY_GPIOA, 0);
        hsai.device[DSY_SAI_2]      = DSY_AUDIO_DEVICE_AK4556;
        hsai.samplerate[DSY_SAI_2]  = DSY_AUDIO_SAMPLERATE_48K;
        hsai.bitdepth[DSY_SAI_2]    = DSY_AUDIO_BITDEPTH_24;
        hsai.a_direction[DSY_SAI_2] = DSY_AUDIO_TX;
        hsai.b_direction[DSY_SAI_2] = DSY_AUDIO_RX;
        hsai.sync_config[DSY_SAI_2] = DSY_AUDIO_SYNC_MASTER;
    }

	// Other
	haudio.dev0_i2c = NULL;
	haudio.dev1_i2c = NULL;


	// Init and Prepare
	dsy_audio_init(&haudio);
    dsy_audio_set_blocksize(DSY_AUDIO_INTERNAL, _blocksize); 
    if (_device == DAISY_PATCH)
    {
        // Reset External Codec
        ak4556_reset_pin.pin  = dsy_pin(PORT_AK4556_RESET, PIN_AK4556_RESET);
        ak4556_reset_pin.mode = DSY_GPIO_MODE_OUTPUT_PP;
        ak4556_reset_pin.pull = DSY_GPIO_NOPULL;
        dsy_gpio_init(&ak4556_reset_pin);
        dsy_audio_set_blocksize(DSY_AUDIO_EXTERNAL, _blocksize); 
        dsy_gpio_write(&ak4556_reset_pin, 0);
        delay(1);
        dsy_gpio_write(&ak4556_reset_pin, 1);
    }

    ConfigureSdram();
    
    DaisyHardware hw;
    hw.Init(get_callbackrate(), device);

    return hw;
}

void AudioClass::begin(DaisyDuinoCallback cb)
{
    if (_device == DAISY_PATCH)
    {
        // Start both channels, and set first callback
        dsy_audio_set_mc_callback(cb);
        dsy_audio_start(DSY_AUDIO_INTERNAL);
        dsy_audio_start(DSY_AUDIO_EXTERNAL);
    }
    else
    {
        dsy_audio_set_mc_callback(cb);
        dsy_audio_start(DSY_AUDIO_INTERNAL);
    }
}

void AudioClass::end()
{
    dsy_audio_stop(DSY_AUDIO_INTERNAL);
    if (_device == DAISY_PATCH)
    {
        dsy_audio_stop(DSY_AUDIO_EXTERNAL);
    }
}

float AudioClass::get_samplerate()
{
    switch(_samplerate)
    {
        case AUDIO_SR_48K:
            return 48000.0f;
        case AUDIO_SR_96K:
            return 96000.0f;
        default:
            return 48000.0f;
    }
}
