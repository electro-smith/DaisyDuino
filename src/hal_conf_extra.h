// These things should be added to the variant.h file within STM32Duino
// DATA_CACHE_ENABLE requires an update to the linker script (detailed below).
#define HAL_SDRAM_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_MDMA_MODULE_ENABLED
#define INSTRUCTION_CACHE_ENABLE

// In order for the Cache to work and still allow the DMA to work 
// for audio (without a bunch of cache-maintenance) we need to add 
// the following to the linker script:
/*
.sram1_bss (NOLOAD) :
{
    . = ALIGN(4);
    _ssram1_bss = .;
    PROVIDE(__sram1_bss_start = _ssram1_bss);
    *(.sram1_bss)
    *(.sram1_bss*)
    . = ALIGN(4);
    _esram1_bss = .;
    PROVIDE(__sram1_bss_end = _esram1_bss);
} >RAM_D2
*/

// Once that is added we can repalce
//#define DATA_CACHE_ENABLE

