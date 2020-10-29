#ifndef DAISYDUINO_MPU
#define DAISYDUINO_MPU

#ifdef __cplusplus
extern "C"
{
#endif

// Initializes, and configures MPU to: 
// - mark DMA buffer area for SAI as cacheless
// - marks the SDRAM as cacheable
void dsy_mpu_init();

#ifdef __cplusplus
}
#endif
#endif
