#include <stdio.h>
#include "com.h"
#include "USART.h"
#include "ringbuffer.h"
#include "control_new.h"
#include "IORedefine.h"
#include "motor.h"
#include "data_input.h"
#include "actuator_config.h"

#define SLAVER_PACKAGE_SIZE     6 //2+2+2
#define TX_FRAME_SIZE           17//2+10+2+1+2
#define START_BYTE_A    0xa5
#define START_BYTE_B    0x5a

extern u16 ircmd;
u8 ircmd_decode_ok;
u8 MasterTxFrame[TX_FRAME_SIZE];
static u16 GetCheckSum(u8* pbuffer, u8 len);
static MasterTxData_TypeDef MasterTxData;
MasterTxData_TypeDef* pMasterTxData = &MasterTxData;


static void SlaverData_Decode(const u8* pbuffer);
static u8 SlaverData_CheckSum(const u8* pbuffer,u8 length);


/* 发送忙标志，用来控制缓冲区第一数据的发送使能 */
u8 flag_tx_busy = 0;

/* 从机接收环形缓冲区定义 */
RingBuffer_TypeDef SlaverRx_RingBuffer;
u8 Slaver_RX_Buffer[64];

/* 主机发送环形缓冲区定义 */
RingBuffer_TypeDef MasterTx_RingBuffer;
u8 Master_TX_Buffer[256];


/********************************** 串口1配置函数 *******************************/
static void UART1_Config(void)
{
    
	COMx_InitDefine		COMx_InitStructure;					//ޡٹ֨ӥ
	EA = 0;
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹԃҨ͘Ê,   BRT_Timer1, BRT_Timer2 (עӢ: Ԯࠚ2ڌ֨ʹԃBRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//Ҩ͘Ê, һѣ 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//ޓ˕Պѭ,   ENABLEܲDISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//Ҩ͘ÊݓѶ, ENABLEܲDISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//א׏Պѭ,   ENABLEܲDISABLE
	COMx_InitStructure.UART_Polity    = PolityHigh;			//א׏ԅЈܶ, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P16_P17;	//Ȑۻ׋ࠚ,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(ҘѫʹԃŚҿʱד)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//Śҿ׌·RXDԫTXD, ضא݌, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//ԵʼۯԮࠚ1 USART1,USART2
    EA = 1;
} 

static void UART1Debug_Config()
{
     
	COMx_InitDefine		COMx_InitStructure;					//ޡٹ֨ӥ
	EA = 0;
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹԃҨ͘Ê,   BRT_Timer1, BRT_Timer2 (עӢ: Ԯࠚ2ڌ֨ʹԃBRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//Ҩ͘Ê, һѣ 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//ޓ˕Պѭ,   ENABLEܲDISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//Ҩ͘ÊݓѶ, ENABLEܲDISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//א׏Պѭ,   ENABLEܲDISABLE
	COMx_InitStructure.UART_Polity    = PolityHigh;			//א׏ԅЈܶ, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//Ȑۻ׋ࠚ,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(ҘѫʹԃŚҿʱד)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//Śҿ׌·RXDԫTXD, ضא݌, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//ԵʼۯԮࠚ1 USART1,USART2
    EA = 1;  
    
}

void UART1_SWToDebug()
{
    UART1Debug_Config();
}

void UART1_SWToLCD()
{
    UART1_Config(); 
}


/* 通信配置函数 */
void Communication_Config()
{
    UART1_Config();
    RingBuffer_Init(&SlaverRx_RingBuffer,Slaver_RX_Buffer,sizeof(Slaver_RX_Buffer));
    RingBuffer_Init(&MasterTx_RingBuffer,Master_TX_Buffer,sizeof(Master_TX_Buffer));
}
/**
 ****************************************
 * 函数名：GetMasterTxData()
 * 参数：
 * 返回值： MasterTxData_TypeDef*
 * 功能描述：获取要发送的数据指针
 *
 *****************************************
 */
void GetMasterTxData()
{
    pMasterTxData->uc_current_input   =   (u8)((pData_Acquire->current_input)*10.0+0.5);
    pMasterTxData->uc_opening         =   (u8)((pData_Acquire->opening)*100.0+0.5);
    pMasterTxData->motor_cr           =   Motor_CR;
    pMasterTxData->valve_position     =   LimitPosition_Output();
    pMasterTxData->uint_voltage_valve =   (u16)(pData_Voltage->voltage_valve*100.0+0.5);
    pMasterTxData->sen                =   pSystemParam->sen;
    pMasterTxData->workmode           =   mode;
    pMasterTxData->motor_stat         =   motor_status;
    pMasterTxData->encoder_flag       =   (u8)use_encoder_flag;
    pMasterTxData->uint_encoder_value =   (u16)pData_ADC->adcvalue_encoder;
    
}

/**
 *****************************************************************************************************
 * 函数名：GetMasterTxFrame()
 * 参数：
 * 功能描述：将发送的数据组成数据帧
 * Byte1 | Byte2 |       Byte3      |    Byte4   |   Byte5  |     Byte6     |        Byte7        |
 * ------+-------+------------------+------------+----------+---------------+---------------------+
 * 0x5A  | 0xA5  | uc_current_input | uc_opening | motor_cr |valve_position | uint_voltage_valveH |
 * ================================================================================================
 *        Byte8        |   Byte9  |   Byte10  |  Byte11  |    Byte12     |   Byte13  |   Byte14   |
 * --------------------+----------+-----------+----------+---------------+-----------+------------+
 * uint_voltage_valveL |   senH   |   senL    |   mode   |  motor_status |   sumH    |    sumL    |
 ******************************************************************************************************
 */
static void UpdateMasterTxFrame()
{
    u8 i = 0;
    u16 sum = 0;
    GetMasterTxData();
    
    MasterTxFrame[0] =  0xA5;
    MasterTxFrame[1] =  0x5A;
#ifdef KAIGUAN_MODE
	  MasterTxFrame[2] = 0;
#else
    MasterTxFrame[2] =  pMasterTxData->uc_current_input;
#endif

    MasterTxFrame[3] =  pMasterTxData->uc_opening;
    MasterTxFrame[4] =  pMasterTxData->motor_cr;
    MasterTxFrame[5] =  pMasterTxData->valve_position;
    MasterTxFrame[6] =  (u8)(pMasterTxData->uint_voltage_valve>>8 & 0x00FF);
    MasterTxFrame[7] =  (u8)(pMasterTxData->uint_voltage_valve & 0x00FF);
    MasterTxFrame[8] =  (u8)(pMasterTxData->sen>>8 & 0x00FF);
    MasterTxFrame[9] =  (u8)(pMasterTxData->sen & 0x00FF);
    MasterTxFrame[10] = pMasterTxData->workmode;
    MasterTxFrame[11] = pMasterTxData->motor_stat;
    MasterTxFrame[12] = pMasterTxData->encoder_flag;
    MasterTxFrame[13] = (u8)(pMasterTxData->uint_encoder_value>>8 & 0x00FF);
    MasterTxFrame[14] = (u8)(pMasterTxData->uint_encoder_value & 0x00FF);
    
    sum = GetCheckSum(MasterTxFrame,15);    
    MasterTxFrame[15] = (u8)(sum>>8 & 0x00FF);
    MasterTxFrame[16] = (u8)(sum & 0x00FF);

}

/**
 **************************************************
 * 函数名：GetCheckSum
 * 参数1：u8* pbuffer
 * 参数2：u8 len
 * 返回值：u16 sum
 * 功能描述：获取校验和
 *
 ****************************************************
 */
static u16 GetCheckSum(u8* pbuffer, u8 len)
{
    u8 i;
    u16 sum = 0;
    for(i=0;i<len;i++)
    {
        sum += pbuffer[i];
    }
    return sum;
          
}
/**
 **************************************************
 * 函数名：TransmitFrameToSlaver
 * 参数：
 * 返回值：
 * 功能描述：获取发送的数据帧并写入发送Ringbuffer中
 *
 ****************************************************
 */
void TransmitFrameToSlaver()
{
    //u8 flag_pushframe_ok;

    if(0 == flag_tx_busy )//&& flag_pushframe_ok == 1)
    {
        UART1_SWToLCD();
        UpdateMasterTxFrame();
        PushFrameToTxRingBuffer(&MasterTx_RingBuffer,MasterTxFrame,sizeof(MasterTxFrame));
        flag_tx_busy = 1;
        TI=1;        
    }

    
}

//void Uart1Sendrb()
//{
//    
//    
//}

/**
 **************************************************
 * 函数名：SlaverIsReadyToRead
 * 参数：
 * 返回值：0 从机接收ringbuffer为空
 *         1 从机接收ringbuffer非空
 * 功能描述：判断从机接收ringbuffer是否有数据可读
 *
 ****************************************************
 */
u8 SlaverIsReadyToRead()
{
    return !RingBufferIsEmpty(&SlaverRx_RingBuffer);      
}

/**
 **************************************************
 * 函数名：SlaverData_Update
 * 参数：
 * 返回值：
 * 功能描述：匹配从机的接收序列并解码数据
 *
 ****************************************************
 */
void SlaverData_Update()
{
    static u8 Slaver_WorkingBuffer[32];
    static u8 Slaver_WorkingBuffer_cnt = 0;
    u8 cur;
    
    while(!RingBufferIsEmpty(&SlaverRx_RingBuffer))
    {
        cur = PopByteFromRxRingBuffer(&SlaverRx_RingBuffer);
        if(Slaver_WorkingBuffer_cnt == 0)
        {
            Slaver_WorkingBuffer[0]=Slaver_WorkingBuffer[1];
            Slaver_WorkingBuffer[1]=cur;
            if(Slaver_WorkingBuffer[0]==START_BYTE_A && Slaver_WorkingBuffer[1]==START_BYTE_B)
            {
                Slaver_WorkingBuffer_cnt = 2;
            }
        }
        else
        {   
            Slaver_WorkingBuffer[Slaver_WorkingBuffer_cnt++] = cur;
        }
        
        if(Slaver_WorkingBuffer_cnt >= SLAVER_PACKAGE_SIZE)
        {
            SlaverData_Decode(Slaver_WorkingBuffer);
            Slaver_WorkingBuffer_cnt = 0;
        }
    }   
    
}

/**
 **************************************************
 * 函数名：SlaverData_Decode
 * 参数：const u8* pbuffer
 * 返回值：
 * 功能描述：对匹配到的一帧接收数据进行解码
 *
 ****************************************************
 */
static void SlaverData_Decode(const u8* pbuffer)
{
    if(SlaverData_CheckSum(pbuffer,SLAVER_PACKAGE_SIZE))
    {
        ircmd = (((u16)pbuffer[2])<<8 & 0xff00) | ((u16)pbuffer[3] & 0x00ff);
        ircmd_decode_ok = 1;
    }        
      
}

/**
 **************************************************
 * 函数名：SlaverData_CheckSum
 * 参数1：const u8* pbuffer
 * 参数2：u8 length
 * 返回值：1 校验正确
 *         0 校验失败
 * 功能描述：计算校验和
 *
 ****************************************************
 */
static u8 SlaverData_CheckSum(const u8* pbuffer,u8 length)
{
    u16 sum = 0;
    u16 sumbuffer = 0;
    u8 i;
    sumbuffer = ((((u16)pbuffer[length-2])<<8) & 0xff00) | ((u16)pbuffer[length-1] & 0x00ff);
    for(i=0;i<length-2;i++)
    {
        sum += pbuffer[i];
    }
    if(sum==sumbuffer) return 1;
    else return 0;
      
}

/********************* UART1中断服务函数************************/
void UART1_Handler (void) interrupt UART1_VECTOR
{
	u8 rxdata;
    if(RI)
	{
		RI = 0;
        rxdata = SBUF;
        PushByteToRxRingBuffer(&SlaverRx_RingBuffer,rxdata);
	}
    else if(TI)
	{
		TI = 0;
        if(0 == RingBufferIsEmpty(&MasterTx_RingBuffer))
        {
            SBUF = PopByteFromTxRingBuffer(&MasterTx_RingBuffer);
        }
        else flag_tx_busy = 0;

	}
}

char putchar (char c)   
{        
    ES=0;        
    SBUF = c;        
    while(TI==0);        
    TI=0;        
    ES=1;        
    return 0;
}

