

#include "uart.h"



/* å‘é€å¿™æ ‡å¿—ï¼Œç”¨æ¥æ§åˆ¶ç¼“å†²åŒºç¬¬ä¸€æ•°æ®çš„å‘é€ä½¿èƒ½ */
u8 flag_tx_busy;

RingBuffer_TypeDef SlaverRx_RingBuffer;
u8 Slaver_RX_Buffer[64];

RingBuffer_TypeDef MasterTx_RingBuffer;
u8 Master_TX_Buffer[64];





/**********************************ä¸²å£1é…ç½®*******************************/
static void UART1_Config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//½á¹¹¶¨Òå
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//Ä£Ê½,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//Ê¹ÓÃ²¨ÌØÂÊ,   BRT_Timer1, BRT_Timer2 (×¢Òâ: ´®¿Ú2¹Ì¶¨Ê¹ÓÃBRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//²¨ÌØÂÊ, Ò»°ã 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//½ÓÊÕÔÊĞí,   ENABLE»òDISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//²¨ÌØÂÊ¼Ó±¶, ENABLE»òDISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//ÖĞ¶ÏÔÊĞí,   ENABLE»òDISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//ÖĞ¶ÏÓÅÏÈ¼¶, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P16_P17;	//ÇĞ»»¶Ë¿Ú,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(±ØĞëÊ¹ÓÃÄÚ²¿Ê±ÖÓ)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//ÄÚ²¿¶ÌÂ·RXDÓëTXD, ×öÖĞ¼Ì, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//³õÊ¼»¯´®¿Ú1 USART1,USART2
    //TX1_write2buff(0);
	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1·¢ËÍÒ»¸ö×Ö·û´®
}  

/********************* UART1ä¸­æ–­æœåŠ¡å‡½æ•°************************/
void UART1_Handler (void) interrupt UART1_VECTOR
{
	u8 rxdata;
    if(RI)
	{
		RI = 0;
        rxdata = SBUF;
        PushByteToRingBuffer(&SlaverRx_RingBuffer,rxdata);
	}

	if(TI)
	{
		TI = 0;
        if(0 == RingBufferIsEmpty(&MasterTx_RingBuffer))
        {
            SBUF = PopByteFromRingBuffer(&MasterTx_RingBuffer);
        }
        else flag_tx_busy = 0;

	}
}

void Communication_Config()
{
    UART1_Config();
    RingBuffer_Init(&SlaverRx_RingBuffer,Slaver_RX_Buffer,sizeof(Slaver_RX_Buffer));
    RingBuffer_Init(&MasterTx_RingBuffer,Master_TX_Buffer,sizeof(Master_TX_Buffer));
}

