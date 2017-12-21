#include "ringbuffer.h"

/* ringbuffer的大小是2的幂次方 */
#define POWER_TWO

/**********************************************************  
* 函数名：RingBuffer_Init
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 参数2：u8* buff
* 参数3：u8 length
* 返回值：无
* 功能描述：初始化RingBuffer
* 
***********************************************************/
void RingBuffer_Init(RingBuffer_TypeDef* pRingBuffer, u8* buff, u8 length)
{
    pRingBuffer->pbuffer = buff;
	pRingBuffer->buffersize = length;
	pRingBuffer->write = 0;
	pRingBuffer->read = 0;
}
/**********************************************************  
* 函数名：ClearRingBuffer
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 返回值：无
* 功能描述：清除RingBuffer
* 
***********************************************************/
//void ClearRingBuffer(RingBuffer_TypeDef* pRingBuffer)
//{
//	pRingBuffer->write = 0;
//	pRingBuffer->read = 0;
//	
//}

/**********************************************************  
* 函数名：PushByteToRingBuffer
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 参数2：u8 value
* 返回值：0 缓冲区满
*         1 写入成功
* 功能描述：将一个字节的数据写入RingBuffer
* 
***********************************************************/
u8 PushByteToTxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8 value)  
{

#ifdef POWER_TWO
        u8 write_next = (pRingBuffer->write + 1)&(pRingBuffer->buffersize - 1);
    #else
        u8 write_next = (pRingBuffer->write + 1)%(pRingBuffer->buffersize);
#endif
	
	if(pRingBuffer->read != write_next)
	{
		(pRingBuffer->pbuffer)[pRingBuffer->write] = value;
		pRingBuffer->write = write_next;
        return 1;        
	}
	else    return 0; 

}
u8 PushByteToRxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8 value)  
{

#ifdef POWER_TWO
        u8 write_next = (pRingBuffer->write + 1)&(pRingBuffer->buffersize - 1);
    #else
        u8 write_next = (pRingBuffer->write + 1)%(pRingBuffer->buffersize);
#endif
	
	if(pRingBuffer->read != write_next)
	{
		(pRingBuffer->pbuffer)[pRingBuffer->write] = value;
		pRingBuffer->write = write_next;
        return 1;        
	}
	else    return 0; 

}
/**********************************************************  
* 函数名：PushFrameToRingBuffer
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 参数2：u8* frame
* 参数3：u8 framesize
* 返回值：0 缓冲区可写区小于帧长度
*         1 帧写入成功
* 功能描述：将一帧数据写入RingBuffer
* 
***********************************************************/
u8 PushFrameToTxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8* frame,u8 framesize)
{

    if(GetFreeSpaceOfRingBuffer(pRingBuffer) > framesize)
    {
        u8 i;
        for(i=0;i<framesize;i++)
        {
            PushByteToTxRingBuffer(pRingBuffer,frame[i]);   
        }
        return 1; 
        
    }
    else return 0;   
}
//u8 PushFrameToRxRingBuffer(RingBuffer_TypeDef* pRingBuffer,u8* frame,u8 framesize)
//{

//    if(GetFreeSpaceOfRingBuffer(pRingBuffer) > framesize)
//    {
//        u8 i;
//        for(i=0;i<framesize;i++)
//        {
//            PushByteToRxRingBuffer(pRingBuffer,frame[i]);   
//        }
//        return 1; 
//        
//    }
//    else return 0;   
//}
/**********************************************************  
* 函数名：PopByteFromRingBuffer
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 返回值：u8 
*         0xff 缓冲区空
* 功能描述：从RingBuffer读取一个字节
* 
***********************************************************/
u8 PopByteFromTxRingBuffer(RingBuffer_TypeDef* pRingBuffer) 
{

#ifdef POWER_TWO
        u8 read_next = (pRingBuffer->read + 1)&(pRingBuffer->buffersize - 1);
    #else
        u8 read_next = (pRingBuffer->read + 1)%(pRingBuffer->buffersize);
#endif
	u8 temp = 0;
	
	if(pRingBuffer->write == pRingBuffer->read)	return 0xff;// ringbuffer is empty
	 
    temp = (pRingBuffer->pbuffer)[pRingBuffer->read];
    pRingBuffer->read = read_next;
    return temp;
	
}
u8 PopByteFromRxRingBuffer(RingBuffer_TypeDef* pRingBuffer) 
{

#ifdef POWER_TWO
        u8 read_next = (pRingBuffer->read + 1)&(pRingBuffer->buffersize - 1);
    #else
        u8 read_next = (pRingBuffer->read + 1)%(pRingBuffer->buffersize);
#endif
	u8 temp = 0;
	
	if(pRingBuffer->write == pRingBuffer->read)	return 0xff;// ringbuffer is empty
	 
    temp = (pRingBuffer->pbuffer)[pRingBuffer->read];
    pRingBuffer->read = read_next;
    return temp;
	
}
/**********************************************************  
* 函数名：RingBufferIsEmpty
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 返回值：1 RingBuffer 空
*         0 Ringbuffer非空
* 功能描述：判断Ringbuffer是否空
* 
***********************************************************/
u8 RingBufferIsEmpty(RingBuffer_TypeDef* pRingBuffer)
{
	if(pRingBuffer->write == pRingBuffer->read)
    return 1;
    else    return 0;	
}
/**********************************************************  
* 函数名：RingBufferIsFull
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 返回值：1 RingBuffer 满
*         0 Ringbuffer 非满
* 功能描述：判断Ringbuffer是否满
* 
***********************************************************/
//u8 RingBufferIsFull(RingBuffer_TypeDef* pRingBuffer)
//{
//#ifdef POWER_TWO
//    return (((pRingBuffer->write + 1) & (pRingBuffer->buffersize-1)) == pRingBuffer->read);
//    #else
//    return (((pRingBuffer->write + 1) % (pRingBuffer->buffersize)) == pRingBuffer->read);
//#endif	
//}

/**********************************************************  
* 函数名：GetCountOfRingBuffer
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 返回值：u8
* 功能描述：获得RingBuffer中的未读数据量
* 
***********************************************************/
u8 GetCountOfRingBuffer(RingBuffer_TypeDef* pRingBuffer)
{
    return (u8)((pRingBuffer->write - pRingBuffer->read + pRingBuffer->buffersize) % pRingBuffer->buffersize);   
}

/**********************************************************  
* 函数名：GetCountOfRingBuffer
* 参数1：RingBuffer_TypeDef* pRingBuffer
* 返回值：u8
* 功能描述：获得RingBuffer中的未读数据量
* 
***********************************************************/
u8 GetFreeSpaceOfRingBuffer(RingBuffer_TypeDef* pRingBuffer)
{
    u8 temp;
    temp = GetCountOfRingBuffer(pRingBuffer);
    return (u8)(pRingBuffer->buffersize - temp - 1);
    
}

