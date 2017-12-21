

#include "uart.h"



/* 发送忙标志，用来控制缓冲区第一数据的发送使能 */
u8 flag_tx_busy;

RingBuffer_TypeDef SlaverRx_RingBuffer;
u8 Slaver_RX_Buffer[64];

RingBuffer_TypeDef MasterTx_RingBuffer;
u8 Master_TX_Buffer[64];





/**********************************串口1配置*******************************/
static void UART1_Config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P16_P17;	//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//��ʼ������1 USART1,USART2
    //TX1_write2buff(0);
	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1����һ���ַ���
}  

/********************* UART1中断服务函数************************/
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

