/**
  ******************************************************************************
  * @file           : usbd_midi_if.h
  * @brief          : Header for usbd_midi_if file.
  ******************************************************************************
*/

#ifndef __USBD_MIDI_IF_H
#define __USBD_MIDI_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_midi.h"
#include "usbd_desc.h"

extern USBD_MIDI_ItfTypeDef  USBD_Interface_fops_FS;

extern void sendMidiMessage(uint8_t *msg, uint16_t size);
extern uint8_t USBD_MIDI_SendData (USBD_HandleTypeDef *pdev, uint8_t *pBuf, uint16_t length);

// from mi:muz (Interface functions)
extern void mimuz_init(void);
extern void setHdlNoteOff(void (*fptr)(uint8_t ch, uint8_t note, uint8_t vel));
extern void setHdlNoteOn(void (*fptr)(uint8_t ch, uint8_t note, uint8_t vel));
extern void setHdlCtlChange(void (*fptr)(uint8_t ch, uint8_t num, uint8_t value));

extern void sendNoteOn(uint8_t ch, uint8_t note, uint8_t vel);
extern void sendNoteOff(uint8_t ch, uint8_t note);
extern void sendCtlChange(uint8_t ch, uint8_t num, uint8_t value);

// Call in main loop
extern void processMidiMessage(void);
extern void USBD_MIDI_SendPacket(void);

#ifdef __cplusplus
}
#endif
  
#endif /* __USBD_MIDI_IF_H */
