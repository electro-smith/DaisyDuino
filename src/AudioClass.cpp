#include "AudioClass.h"
#include "DaisyDuino.h"

#include "utility/dma.h"
#include "utility/gpio.h"
#include "utility/sys_mpu.h"
#include "utility/codec_wm8731.h"
#include "utility/codec_ak4556.h"

#include <Wire.h>

//*** I2C ***
// This is a workaround to support a missing pin within the DaisySeed variant
#ifdef ARDUINO_DAISY_SEED
#ifdef HAL_I2C_MODULE_ENABLED
const PinMap PinMap_I2C_SDA[] = {
  {PB_7_ALT1, I2C1, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C1)},
  {PB_7,      I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF6_I2C4)},
  {PB_9,      I2C1, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C1)},
  {PB_9_ALT1, I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF6_I2C4)},
  {PB_11,     I2C2, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C2)},
  // {PC_9,      I2C3, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C3)},
  // {PD_13,     I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C4)},
  // {PF_0,      I2C2, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C2)},
  // {PF_15,     I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C4)},
  // {PH_5,      I2C2, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C2)},
  // {PH_8,      I2C3, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C3)},
  // {PH_12,     I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C4)},
  {NC,        NP,   0}
};
#endif //ifdef HAL_I2C_MODULE_ENABLED

#ifdef HAL_I2C_MODULE_ENABLED
const PinMap PinMap_I2C_SCL[] = {
  // {PA_8,      I2C3, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C3)},
  {PB_6_ALT1, I2C1, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C1)},
  {PB_6,      I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF6_I2C4)},
  {PB_8,      I2C1, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C1)},
  {PB_8_ALT1, I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF6_I2C4)},
  // {PB_10,     I2C2, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C2)},
  // {PD_12,     I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C4)},
  // {PF_1,      I2C2, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C2)},
  // {PF_14,     I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C4)},
  {PH_4,      I2C2, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C2)},
  // {PH_7,      I2C3, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C3)},
  // {PH_11,     I2C4, STM_PIN_DATA(STM_MODE_AF_OD, GPIO_NOPULL, GPIO_AF4_I2C4)},
  {NC,        NP,   0}
};
#endif //ifdef HAL_I2C_MODULE_ENABLED
#endif // ifdef ARDUINO_DAISY_SEED

using namespace daisy;

dsy_gpio ak4556_reset_pin;

AudioClass DAISY;
TwoWire daisy_i2c2;

void AudioClass::ConfigureSdram() {
  dsy_gpio_pin *pin_group;
  sdram_handle.state = DSY_SDRAM_STATE_ENABLE;
  pin_group = sdram_handle.pin_config;
  pin_group[DSY_SDRAM_PIN_SDNWE] = dsy_pin(DSY_GPIOH, 5);

  dsy_sdram_init(&sdram_handle);
}

DaisyHardware AudioClass::init(DaisyDuinoDevice device,
                               DaisyDuinoSampleRate sr) {
  // convert DaisyDuino sr to SaiHandle sr to ensure bwd compatibility
  SaiHandle::Config::SampleRate sample_rate;

  switch (sr) {
  case AUDIO_SR_8K:
    sample_rate = SaiHandle::Config::SampleRate::SAI_8KHZ;
    break;
  case AUDIO_SR_16K:
    sample_rate = SaiHandle::Config::SampleRate::SAI_16KHZ;
    break;
  case AUDIO_SR_32K:
    sample_rate = SaiHandle::Config::SampleRate::SAI_32KHZ;
    break;
  case AUDIO_SR_48K:
    sample_rate = SaiHandle::Config::SampleRate::SAI_48KHZ;
    break;
  case AUDIO_SR_96K:
    sample_rate = SaiHandle::Config::SampleRate::SAI_96KHZ;
    break;
  default:
    sample_rate = SaiHandle::Config::SampleRate::SAI_48KHZ;
  }

  // Set Audio Device, num channels, etc.
  // Only difference is Daisy Patch has second AK4556 and 4 channels
  HAL_Init();
  //SCB_DisableDCache(); // Still needs to wait for linker..
  _device = device;
  dsy_dma_init(); // may interfere with core STM32 Arduino stuff...
  dsy_mpu_init();

  // Set up audio
  // SAI1
  SaiHandle::Config sai_config[2];
  sai_config[0].periph = SaiHandle::Config::Peripheral::SAI_1;
  sai_config[0].sr = sample_rate;
  sai_config[0].bit_depth = SaiHandle::Config::BitDepth::SAI_24BIT;
  sai_config[0].a_sync = SaiHandle::Config::Sync::MASTER;
  sai_config[0].b_sync = SaiHandle::Config::Sync::SLAVE;
  sai_config[0].pin_config.fs = {DSY_GPIOE, 4};
  sai_config[0].pin_config.mclk = {DSY_GPIOE, 2};
  sai_config[0].pin_config.sck = {DSY_GPIOE, 5};
  sai_config[0].pin_config.sa = {DSY_GPIOE, 6};
  sai_config[0].pin_config.sb = {DSY_GPIOE, 3};

  #ifdef ARDUINO_DAISY_SEED
  // which seed version are we on?
  switch(BoardVersionCheck())
  {
    case BoardVersion::DAISY_SEED_1_1:
    {
      // Data Line Directions
      sai_config[0].a_dir         = SaiHandle::Config::Direction::RECEIVE;
      sai_config[0].b_dir         = SaiHandle::Config::Direction::TRANSMIT;

      Wm8731::Config codec_cfg;
      codec_cfg.Defaults();
      Wm8731 codec;
      codec.Init(codec_cfg, &daisy_i2c2);
    }
    break;

    case BoardVersion::DAISY_SEED:
    default:
    {
      // Data Line Directions
      sai_config[0].a_dir         = SaiHandle::Config::Direction::TRANSMIT;
      sai_config[0].b_dir         = SaiHandle::Config::Direction::RECEIVE;
      
      Ak4556 codec;
      codec.Init({DSY_GPIOB, 11});
    }
    break;
  }

  #endif

  //patch SM flips these
  if(_device == DAISY_PATCH_SM){
    sai_config[0].a_dir = SaiHandle::Config::Direction::RECEIVE;
    sai_config[0].b_dir = SaiHandle::Config::Direction::TRANSMIT;
  }

  // Then Initialize
  SaiHandle sai_handle[2];
  sai_handle[0].Init(sai_config[0]);

  if(_device == DAISY_PATCH_SM){
    Pcm3060 codec;
    codec.Init(&daisy_i2c2);
  }

  // SAI2
  if (_device == DAISY_PATCH) {
    sai_config[1].periph = SaiHandle::Config::Peripheral::SAI_2;
    sai_config[1].sr = sample_rate;
    sai_config[1].bit_depth = SaiHandle::Config::BitDepth::SAI_24BIT;
    sai_config[1].a_sync = SaiHandle::Config::Sync::SLAVE;
    sai_config[1].b_sync = SaiHandle::Config::Sync::MASTER;
    sai_config[1].a_dir = SaiHandle::Config::Direction::TRANSMIT;
    sai_config[1].b_dir = SaiHandle::Config::Direction::RECEIVE;
    sai_config[1].pin_config.fs = {DSY_GPIOG, 9};
    sai_config[1].pin_config.mclk = {DSY_GPIOA, 1};
    sai_config[1].pin_config.sck = {DSY_GPIOA, 2};
    sai_config[1].pin_config.sb = {DSY_GPIOA, 0};
    sai_config[1].pin_config.sa = {DSY_GPIOD, 11};

    sai_handle[1].Init(sai_config[1]);
  }

  AudioHandle::Config cfg;
  cfg.blocksize = 48;
  cfg.samplerate = sample_rate;

  if (_device == DAISY_PATCH) {
    // Reset External Codec
    Ak4556 codec;
    codec.Init({DSY_GPIOB, 14});

    cfg.postgain = 0.5f;
    audio_handle.Init(cfg, sai_handle[0], sai_handle[1]);
  }

  else {
    cfg.postgain = 1.f;
    audio_handle.Init(cfg, sai_handle[0]);
  }

  ConfigureSdram();

  DaisyHardware hw;
  callback_rate_ = AudioSampleRate() / AudioBlockSize();

  hw.Init(callback_rate_, device);

  return hw;
}

void AudioClass::begin(AudioHandle::AudioCallback cb) {
  audio_handle.Start(cb);
}

void AudioClass::begin(AudioHandle::InterleavingAudioCallback cb) {
  if (_device == DAISY_PATCH) {
    end();
  }
  audio_handle.Start(cb);
}

void AudioClass::end() { audio_handle.Stop(); }

float AudioClass::get_samplerate() { return audio_handle.GetSampleRate(); }

float AudioClass::get_callbackrate() { return callback_rate_; }

float AudioClass::get_blocksize() { return audio_handle.GetConfig().blocksize; }

void AudioClass::StartAudio(AudioHandle::InterleavingAudioCallback cb) {
  begin(cb);
}

void AudioClass::StartAudio(AudioHandle::AudioCallback cb) { begin(cb); }

void AudioClass::ChangeAudioCallback(
    AudioHandle::InterleavingAudioCallback cb) {
  if (_device == DAISY_PATCH) {
    end();
  }

  audio_handle.ChangeCallback(cb);
}

void AudioClass::ChangeAudioCallback(AudioHandle::AudioCallback cb) {
  audio_handle.ChangeCallback(cb);
}

void AudioClass::StopAudio() { end(); }

void AudioClass::SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate) {
  audio_handle.SetSampleRate(samplerate);
  callback_rate_ = AudioSampleRate() / AudioBlockSize();
}

float AudioClass::AudioSampleRate() { return get_samplerate(); }

void AudioClass::SetAudioBlockSize(size_t blocksize) {
  audio_handle.SetBlockSize(blocksize);
  callback_rate_ = AudioSampleRate() / AudioBlockSize();
}

size_t AudioClass::AudioBlockSize() { return get_blocksize(); }

float AudioClass::AudioCallbackRate() { return get_callbackrate(); }

AudioClass::BoardVersion AudioClass::BoardVersionCheck(){
    /** Version Checks:
     *  * Fall through is Daisy Seed v1 (aka Daisy Seed rev4)
     *  * PD3 tied to gnd is Daisy Seed v1.1 (aka Daisy Seed rev5)
     *  * PD4 tied to gnd reserved for future hardware
     */

    pinMode(PD3, INPUT_PULLUP);

    if(!digitalRead(PD3)){
        return BoardVersion::DAISY_SEED_1_1;
    }
    else{
        return BoardVersion::DAISY_SEED;
    }
}