// Queue32.h
// 4byte ring buffer (32bit version)
//
//    (CC at)2016 by D.F.Mac. @TripArts Music
//
// 2016.07.27 : D.F.Mac.

#ifndef __queue32_h__
#define __queue32_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif 

#define B4QUEUE_SIZE 64

typedef struct{
  uint32_t top;
  uint32_t last;
  uint32_t num;
  uint32_t queue_size;	// not use yet.
  uint32_t queue[B4QUEUE_SIZE];
}stB4Arrq;

void b4arrq_init(stB4Arrq *qp);
uint32_t b4arrq_push(stB4Arrq *qp, uint32_t *p);
uint32_t *b4arrq_pop(stB4Arrq *qp);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
