#ifndef DSY_AUDIO_COM_H
#define DSY_AUDIO_COM_H

// Callback Definition
typedef void (*DaisyAudioCallback)(float**, float**, uint32_t);

void dsy_audio_init();
void dsy_audio_start();
void dsy_audio_stop();

#endif
