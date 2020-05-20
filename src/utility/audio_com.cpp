// And here's where its going to get nuts...
#include "utility/audio_com.h"
#include <stm32h7xx_hal.h>


SAI_HandleTypeDef hsai1_BlockA1;
SAI_HandleTypeDef hsai1_BlockB1;
SAI_HandleTypeDef hsai1_BlockA2;
SAI_HandleTypeDef hsai1_BlockB2;
DMA_HandleTypeDef hdma_sai1_a;
DMA_HandleTypeDef hdma_sai1_b;
DMA_HandleTypeDef hdma_sai2_a;
DMA_HandleTypeDef hdma_sai2_b;

void dsy_audio_init() 
{
}
void dsy_audio_start(DaisyAudioCallback cb) {}
void dsy_audio_stop() {}

