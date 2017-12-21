#ifndef _USART1_H
#define _USART1_H

#include "USART.h"
#include "mode_def.h"
#include "ringbuffer.h"
    

void Communication_Config();
void TransmitFrameToMaster();
void UART2_DebugConfig(void);
u8 MasterIsReadyToRead();
void MasterData_Update();

#endif