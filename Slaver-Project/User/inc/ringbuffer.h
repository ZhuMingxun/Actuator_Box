#ifndef _RINGBUFFER1_H
#define _RINGBUFFER1_H
#ifdef __cplusplus
 extern "C" {
#endif 

#include	"config.h"
   
typedef struct {
    
	u8* pbuffer;
	u8  buffersize;//2^n;
	u8  write;
	u8  read;

}RingBuffer_TypeDef;

void RingBuffer_Init(RingBuffer_TypeDef* pRingBuffer, u8* buff, u8 length);
u8 PushByteToTxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8 value);
u8 PushByteToRxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8 value);
u8 PushFrameToTxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8* frame,u8 framesize);
//u8 PushFrameToRxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8* frame,u8 framesize);
u8 PopByteFromTxRingBuffer(RingBuffer_TypeDef* pRingBuffer);
u8 PopByteFromRxRingBuffer(RingBuffer_TypeDef* pRingBuffer);
u8 RingBufferIsEmpty(RingBuffer_TypeDef* pRingBuffer);
u8 GetCountOfRingBuffer(RingBuffer_TypeDef* pRingBuffer);
u8 GetFreeSpaceOfRingBuffer(RingBuffer_TypeDef* pRingBuffer);
//u8 PushFrameToTxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8* frame,u8 framesize);
//void ClearRingBuffer(RingBuffer_TypeDef* pRingBuffer);
//u8 RingBufferIsFull(RingBuffer_TypeDef* pRingBuffer);
#ifdef __cplusplus
}
#endif

#endif

 



