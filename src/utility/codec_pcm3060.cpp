#include "utility/codec_pcm3060.h"
#include "utility/system.h"

// POWER-ON RESET and EXTERNAL RESET Sequence
// On DaisyPetalSM the RESET pin is held high on hardware,
// and is therefore not able to be reset from sw aside from
// with an I2C -> RESET message.

// Default Format spec for now: I2S 24-bit MSB aligned (FMT1/2[1:0] = 01)

// Slave address: 0b100011Nx
// N = 0 (GND on hardware)
// x = R/W|

// TODO:
// * add Deemphasis configuration
// * add filter-rolloff configuration
// * add format control configuration
// * add digital attenuation for adc/dac


const uint8_t kAddrRegSysCtrl       = 0x40;
const uint8_t kAddrRegDacAttenLeft  = 0x41;
const uint8_t kAddrRegDacAttenRight = 0x42;
const uint8_t kAddrRegDacCtrl1      = 0x43;
const uint8_t kAddrRegDacCtrl2      = 0x44;
const uint8_t kAddrRegDigCtrl       = 0x45;
const uint8_t kAddrRegAdcAttenLeft  = 0x46;
const uint8_t kAddrRegAdcAttenRight = 0x47;
const uint8_t kAddrRegAdcCtrl1      = 0x48;
const uint8_t kAddrRegAdcCtrl2      = 0x49;

// SysCtrl masks
// Reset masks
const uint8_t kMrstBitMask   = 0x80;
const uint8_t kSrstBitMask   = 0x40;
const uint8_t kAdcPsvBitMask = 0x20;
const uint8_t kDacPsvBitMask = 0x10;

// DacCtrl1 Masks
// This can be used for FMT1[1:0] and FMT2[1:0]
const uint8_t kFmtBitMask = 0x03;


namespace daisy
{
Pcm3060::Result Pcm3060::Init(TwoWire* wire)
{
    _wire = wire;

    _wire->begin();
    _wire->setClock(400000L);

    // TODO: bit 1 can be set via hardware and should be configurable.
    dev_addr_ = 0x8c;

    // Reset the codec (though by default we may not need to do this)
    uint8_t sysreg = 0x00;

    // MRST
    if(ReadRegister(kAddrRegSysCtrl, &sysreg) != Result::OK)
        return Result::ERR;
    sysreg &= ~(kMrstBitMask);
    sysreg = 0x40;
    if(WriteRegister(kAddrRegSysCtrl, sysreg) != Result::OK)
        return Result::ERR;
    System::Delay(4);

    // SRST
    if(ReadRegister(kAddrRegSysCtrl, &sysreg) != Result::OK)
        return Result::ERR;
    sysreg &= ~(kSrstBitMask);
    sysreg = 0xB0;
    if(WriteRegister(kAddrRegSysCtrl, sysreg) != Result::OK)
        return Result::ERR;
    System::Delay(4);

    // ADC/DAC Format set to 24-bit LJ
    uint8_t dac_ctrl, adc_ctrl;
    if(ReadRegister(kAddrRegDacCtrl1, &dac_ctrl) != Result::OK)
        return Result::ERR;
    if(ReadRegister(kAddrRegAdcCtrl1, &adc_ctrl) != Result::OK)
        return Result::ERR;
    dac_ctrl |= (kFmtBitMask & 1);
    adc_ctrl |= (kFmtBitMask & 1);
    dac_ctrl = 0x01;
    adc_ctrl = 0x01;
    if(WriteRegister(kAddrRegDacCtrl1, dac_ctrl) != Result::OK)
        return Result::ERR;
    if(WriteRegister(kAddrRegAdcCtrl1, adc_ctrl) != Result::OK)
        return Result::ERR;

    // Disable Powersave for ADC/DAC
    if(ReadRegister(kAddrRegSysCtrl, &sysreg) != Result::OK)
        return Result::ERR;
    sysreg &= ~(kAdcPsvBitMask);
    sysreg &= ~(kDacPsvBitMask);
    sysreg = 0xC0;
    if(WriteRegister(kAddrRegSysCtrl, sysreg) != Result::OK)
        return Result::ERR;

    // Success
    return Result::OK;
}

Pcm3060::Result Pcm3060::ReadRegister(uint8_t addr, uint8_t* data)
{
    _wire->beginTransmission(dev_addr_ >> 1);
    _wire->write(addr);
    _wire->endTransmission(false);
    _wire->requestFrom(dev_addr_ >> 1, 1);
    *data = Wire.read();

    return Result::OK;
}

Pcm3060::Result Pcm3060::WriteRegister(uint8_t addr, uint8_t val)
{
    _wire->beginTransmission(dev_addr_ >> 1);
    _wire->write(addr);
    _wire->write(val);
    _wire->endTransmission();

    return Result::OK;
}

} // namespace daisy
