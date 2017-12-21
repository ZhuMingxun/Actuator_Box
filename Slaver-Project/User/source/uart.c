
#include "uart.h"
#include "LCD.h"

#define MASTER_PACKAGR_SIZE 17 //2+10+3+2
#define SLAVER_PACKAGE_SIZE 6//2+2+2
#define START_BYTE_A    0xa5
#define START_BYTE_B    0x5a

RingBuffer_TypeDef MasterRx_RingBuffer;
u8   Master_RX_Buffer[64];
RingBuffer_TypeDef SlaverTx_RingBuffer;
u8   Slaver_TX_Buffer[64];
static u8 data SlaverTxFrame[SLAVER_PACKAGE_SIZE];
/* å‘é€å¿™æ ‡å¿—ï¼Œç”¨æ¥æ§åˆ¶ç¼“å†²åŒºç¬¬ä¸€æ•°æ®çš„å‘é€ä½¿èƒ½ */
u8 data flag_tx_busy = 0;

extern volatile u16 ircmd;

u8 MasterData_CheckSum(const u8* pbuffer,u8 length);
void MasterData_Decode(u8* pbuffer);

/*************  ´®¿Ú1³õÊ¼»¯º¯Êı *****************/
static void UART1_Config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//½á¹¹¶¨Òå
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//Ä£Ê½,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//Ê¹ÓÃ²¨ÌØÂÊ,   BRT_Timer1, BRT_Timer2 (×¢Òâ: ´®¿Ú2¹Ì¶¨Ê¹ÓÃBRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//²¨ÌØÂÊ, Ò»°ã 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//½ÓÊÕÔÊĞí,   ENABLE»òDISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//²¨ÌØÂÊ¼Ó±¶, ENABLE»òDISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//ÖĞ¶ÏÔÊĞí,   ENABLE»òDISABLE
	COMx_InitStructure.UART_Polity    = PolityHigh;			//ÖĞ¶ÏÓÅÏÈ¼¶, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P16_P17;	//ÇĞ»»¶Ë¿Ú,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(±ØĞëÊ¹ÓÃÄÚ²¿Ê±ÖÓ)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//ÄÚ²¿¶ÌÂ·RXDÓëTXD, ×öÖĞ¼Ì, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//³õÊ¼»¯´®¿Ú1 USART1,USART2
    //TX1_write2buff(0);
	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1·¢ËÍÒ»¸ö×Ö·û´®
}
/*************  ´®¿Ú2³õÊ¼»¯º¯Êı *****************/
void UART2_DebugConfig(void)
{
	COMx_InitDefine		COMx_InitStructure;					//Ş¡Ù¹Ö¨Ó¥
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//Ä£Ê½,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//Ê¹ÔƒÒ¨Í˜ÃŠ,   BRT_Timer1, BRT_Timer2 (×¢Ó¢: Ô®à š2ÚŒÖ¨Ê¹ÔƒBRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//Ò¨Í˜ÃŠ, Ò»Ñ£ 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//Ş“Ë•ÕŠÑ­,   ENABLEÜ²DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//Ò¨Í˜ÃŠİ“Ñ¶, ENABLEÜ²DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//××ÕŠÑ­,   ENABLEÜ²DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//××Ô…ĞˆÜ¶, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//ÈÛ»×‹à š,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(Ò˜Ñ«Ê¹ÔƒÅšÒ¿Ê±×“)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//ÅšÒ¿×ŒÂ·RXDÔ«TXD, Ø¶×İŒ, ENABLE,DISABLE
	USART_Configuration(USART2, &COMx_InitStructure);	   
}

void Communication_Config()
{
    UART1_Config();
    RingBuffer_Init(&MasterRx_RingBuffer,Master_RX_Buffer,sizeof(Master_RX_Buffer));
    RingBuffer_Init(&SlaverTx_RingBuffer,Slaver_TX_Buffer,sizeof(Slaver_TX_Buffer));
}

/**
 **************************************************
 * å‡½æ•°åï¼šGetCheckSum
 * å‚æ•°1ï¼šu8* pbuffer
 * å‚æ•°2ï¼šu8 len
 * è¿”å›å€¼ï¼šu16 sum
 * åŠŸèƒ½æè¿°ï¼šè·å–æ ¡éªŒå’Œ
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
static void UpdateSlaverTxFrame()
{
    u16 checksum = 0;
    SlaverTxFrame[0] = 0xA5;
    SlaverTxFrame[1] = 0x5A;
    SlaverTxFrame[2] = (u8)(ircmd>>8 &0x00ff);
    SlaverTxFrame[3] = (u8)(ircmd & 0x00ff);
    
    checksum = GetCheckSum(SlaverTxFrame,4);
    SlaverTxFrame[4] = (u8)(checksum>>8 &0x00ff);
    SlaverTxFrame[5] = (u8)(checksum & 0x00ff); 
    
}

void TransmitFrameToMaster()
{
    u8 flag_pushframe_ok;
    UpdateSlaverTxFrame();
    flag_pushframe_ok = PushFrameToTxRingBuffer(&SlaverTx_RingBuffer,SlaverTxFrame,sizeof(SlaverTxFrame));
    if(0 == flag_tx_busy && flag_pushframe_ok == 1)
    {
        flag_tx_busy = 1;
        TI=1;
    }
    ircmd = 0; //ÇåÁã·ÀÖ¹ÖØ¸´·¢ËÍ   
}

u8 MasterIsReadyToRead()
{
    return !RingBufferIsEmpty(&MasterRx_RingBuffer);      
}

void MasterData_Update()
{
    static u8 Master_WorkingBuffer[MASTER_PACKAGR_SIZE];
    static u8 Master_WorkingBuffer_cnt = 0;
    u8 cur;
    while(!RingBufferIsEmpty(&MasterRx_RingBuffer))
    {
        cur = PopByteFromRxRingBuffer(&MasterRx_RingBuffer);
        if(Master_WorkingBuffer_cnt == 0)
        {
            Master_WorkingBuffer[0]=Master_WorkingBuffer[1];
            Master_WorkingBuffer[1]=cur;
            if(Master_WorkingBuffer[0]==START_BYTE_A && Master_WorkingBuffer[1]==START_BYTE_B)
            {
                Master_WorkingBuffer_cnt = 2;
            }
        }
        else
        {   
            Master_WorkingBuffer[Master_WorkingBuffer_cnt++] = cur;
        }
        
        if(Master_WorkingBuffer_cnt>=MASTER_PACKAGR_SIZE)
        {
            MasterData_Decode(Master_WorkingBuffer);
            Master_WorkingBuffer_cnt = 0;
        }
    }   
    
}
void MasterData_Decode(const u8* pbuffer)
{
    if(MasterData_CheckSum(pbuffer,MASTER_PACKAGR_SIZE))
    {
        
        pMasterData->current_input      = pbuffer[2]/10.0f;
        pMasterData->opening            = pbuffer[3]/100.0f;
        pMasterData->current_output     = 16.0f * pMasterData->opening +4.0f;
        pMasterData->motor_cr           = pbuffer[4];
        pMasterData->valve_position     = pbuffer[5];
        pMasterData->voltage_valve      = (((u16)pbuffer[6] & 0x00ff)<<8 | ((u16)pbuffer[7] & 0x00ff)) / 100.0f;
        pMasterData->sen                = ((u16)pbuffer[8] & 0x00ff)<<8 | ((u16)pbuffer[9] & 0x00ff);
        pMasterData->mode               = (Mode_Type)pbuffer[10];
        pMasterData->motor_status       = (MotorStatus_Type)pbuffer[11];
        pMasterData->encoder_flag       =  pbuffer[12];
        pMasterData->uint_encoder_value =  (((u16)pbuffer[13] & 0x00ff)<<8) | ((u16)pbuffer[14] & 0x00ff);
        
    }        
      
}

u8 MasterData_CheckSum(const u8* pbuffer,u8 length)
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
/********************* UART1ä¸­æ–­æœåŠ¡å‡½æ•°************************/
void UART1_Handler (void) interrupt UART1_VECTOR
{
	u8 rxdata;
    if(RI)
	{
		RI = 0;
        rxdata = SBUF;
        PushByteToRxRingBuffer(&MasterRx_RingBuffer,rxdata);
	}

	if(TI)
	{
		TI = 0;
        if(0 == RingBufferIsEmpty(&SlaverTx_RingBuffer))
        {
            SBUF = PopByteFromTxRingBuffer(&SlaverTx_RingBuffer);
        }
        else 
        {
            flag_tx_busy = 0;
        }
	}
}



