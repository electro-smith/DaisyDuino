#ifndef DSY_AUDIO_COM_H
#define DSY_AUDIO_COM_H
#define HAL_SAI_MODULE_ENABLED
#include "hid_audio.h"
// Callback Definition
//typedef void (*DaisyAudioCallback)(dsy_audio_callback);
typedef void (*DaisyAudioCallback)(dsy_audio_mc_callback);

void DsyAudioInit();


#endif
