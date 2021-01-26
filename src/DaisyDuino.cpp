#include "DaisyDuino.h"

#include "utility/sai.h"
#include "utility/dma.h"
#include "utility/gpio.h"
#include "utility/sys_mpu.h"

#define PIN_AK4556_RESET 14
#define PORT_AK4556_RESET DSY_GPIOB

using namespace daisy;

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
	//convert DaisyDuino sr to SaiHandle sr to ensure bwd compatibility
	SaiHandle::Config::SampleRate sample_rate;
	
	switch(_samplerate)
    {
        case AUDIO_SR_8K:
            sample_rate = SAI_8KHZ;
			break;
        case AUDIO_SR_16K:
            sample_rate = SAI_16KHZ;
			break;
        case AUDIO_SR_32K:
            sample_rate = SAI_32KHZ;
			break;
        case AUDIO_SR_48K:
            sample_rate = SAI_48KHZ;
			break;
        case AUDIO_SR_96K:
            sample_rate = SAI_96KHZ;
			break;
        default:
			sample_rate_ = SAI_48KHZ;
    }

	
    // Set Audio Device, num channels, etc.
    // Only difference is Daisy Patch has second AK4556 and 4 channels
    HAL_Init();
    SCB_DisableDCache(); // Still needs to wait for linker..
    _samplerate = sr;
    _device = device;
    _blocksize = 48; // default
    dsy_dma_init(); // may interfere with core STM32 Arduino stuff...
    dsy_mpu_init();
    
    // Set up audio
    // SAI1
	SaiHandle::Config sai_config[2];
    sai_config[0].periph          = SaiHandle::Config::Peripheral::SAI_1;
    sai_config[0].sr              = SaiHandle::Config::SampleRate::SAI_48KHZ;
    sai_config[0].bit_depth       = SaiHandle::Config::BitDepth::SAI_24BIT;
    sai_config[0].a_sync          = SaiHandle::Config::Sync::MASTER;
    sai_config[0].b_sync          = SaiHandle::Config::Sync::SLAVE;
    sai_config[0].a_dir           = SaiHandle::Config::Direction::TRANSMIT;
    sai_config[0].b_dir           = SaiHandle::Config::Direction::RECEIVE;
    sai_config[0].pin_config.fs   = {DSY_GPIOE, 4};
    sai_config[0].pin_config.mclk = {DSY_GPIOE, 2};
    sai_config[0].pin_config.sck  = {DSY_GPIOE, 5};
    sai_config[0].pin_config.sa   = {DSY_GPIOE, 6};
    sai_config[0].pin_config.sb   = {DSY_GPIOE, 3};
    // Then Initialize
    SaiHandle sai_handle[2];
    sai_handle[0].Init(sai_config[0]);

	// SAI2
    if (_device == DAISY_PATCH)
    {
		sai_config[1].periph          = SaiHandle::Config::Peripheral::SAI_2;
		sai_config[1].sr              = SaiHandle::Config::SampleRate::SAI_48KHZ;
		sai_config[1].bit_depth       = SaiHandle::Config::BitDepth::SAI_24BIT;
		sai_config[1].a_sync          = SaiHandle::Config::Sync::SLAVE;
		sai_config[1].b_sync          = SaiHandle::Config::Sync::MASTER;
		sai_config[1].a_dir           = SaiHandle::Config::Direction::TRANSMIT;
		sai_config[1].b_dir           = SaiHandle::Config::Direction::RECEIVE;
		sai_config[1].pin_config.fs   = {DSY_GPIOG, 9};
		sai_config[1].pin_config.mclk = {DSY_GPIOA, 1};
		sai_config[1].pin_config.sck  = {DSY_GPIOA, 2};
		sai_config[1].pin_config.sb   = {DSY_GPIOA, 0};
		sai_config[1].pin_config.sa   = {DSY_GPIOD, 11};

		sai_handle[1].Init(sai_config[1]);
    }

    if (_device == DAISY_PATCH)
    {
        // Reset External Codec
        ak4556_reset_pin.pin  = dsy_pin(PORT_AK4556_RESET, PIN_AK4556_RESET);
        ak4556_reset_pin.mode = DSY_GPIO_MODE_OUTPUT_PP;
        ak4556_reset_pin.pull = DSY_GPIO_NOPULL;
        dsy_gpio_init(&ak4556_reset_pin);
        dsy_gpio_write(&ak4556_reset_pin, 0);
        delay(1);
        dsy_gpio_write(&ak4556_reset_pin, 1);
	
		AudioHandle::Config cfg;
		cfg.blocksize  = 48;
		cfg.samplerate = SaiHandle::Config::SampleRate::SAI_48KHZ;
		cfg.postgain   = 0.5f;
		audio_handle.Init(cfg, sai_handle[0], sai_handle[1]);
    }
	
	else{
		AudioHandle::Config cfg;
		cfg.blocksize  = 48;
		cfg.samplerate = SaiHandle::Config::SampleRate::SAI_48KHZ;
		cfg.postgain   = 1.f;
		audio_handle.Init(cfg, sai_handle[0]);
	}
	

    ConfigureSdram();
    
    DaisyHardware hw;
    hw.Init(get_callbackrate(), device);

    return hw;
}

void AudioClass::begin(DaisyDuinoCallback cb)
{
	audio_handle.Start(cb);
}

void AudioClass::end()
{
	audio_handle.Stop();
}

void AudioClass::StartAudio(AudioHandle::InterleavingAudioCallback cb){
}

void AudioClass::StartAudio(AudioHandle::AudioCallback cb){
}

void AudioClass::ChangeAudioCallback(AudioHandle::InterleavingAudioCallback cb){
}

void AudioClass::ChangeAudioCallback(AudioHandle::AudioCallback cb){
}

void AudioClass::StopAudio(){
	end();
}

void AudioClass::SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate){
	audio_handle.SetSampleRate(samplerate);
}

float AudioClass::AudioSampleRate(){
	return get_samplerate();
}

void AudioClass::SetAudioBlockSize(size_t blocksize){
}

size_t AudioClass::AudioBlockSize(){
}

float AudioClass::AudioCallbackRate() const{
}
	

float AudioClass::get_samplerate()
{
    switch(_samplerate)
    {
        case AUDIO_SR_8K:
            return 8000.0f;
        case AUDIO_SR_16K:
            return 16000.0f;
        case AUDIO_SR_48K:
            return 32000.0f;
        case AUDIO_SR_32K:
            return 48000.0f;
        case AUDIO_SR_96K:
            return 96000.0f;
        default:
            return 48000.0f;
    }
}