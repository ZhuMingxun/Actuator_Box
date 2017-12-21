#include "ir.h"
#include "LCD.h"
#include "uart.h"
#include "USART.h"
volatile u16 ircmd = 0;
u16  ircnt = 0;
u8 ircmd_is_rcvd;
static u8 ir_rx_buff[64];
static RingBuffer_TypeDef  IR_RingBuffer;

/**********************Timer0閰嶇疆*******************/
void Timer0_IRConfig(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 100000UL);		//1T 10us涓柇
	TIM_InitStructure.TIM_Run       = DISABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);
    EA = 1;    
}
void IR_Disable()
{
    EA = 0;
    TR0 = 0;
    ET0 = 0;
    EA = 1;   
}
/********************* INT0閰嶇疆鍑芥暟************************/
void Exti0_IRConfig(void)
{
	EXTI_InitTypeDef	EXTI_InitStructure;					//蕖俟吱
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_RiseFall;	//讗讖模式,  	EXT_MODE_RiseFall, EXT_MODE_Fall 
	EXTI_InitStructure.EXTI_Polity    = PolityHigh;			//讗讖詤袌芏,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//讗讖諍循,     ENABLE懿DISABLE
	Ext_Inilize(EXT_INT0,&EXTI_InitStructure);
    EA = 1;
}
/********************* Timer0涓柇鍑芥暟************************/
void Timer0_Handler (void) interrupt TIMER0_VECTOR
{
    ircnt++;
}
/********************* INT0涓柇鍑芥暟************************/
void Exti0_Handler (void) interrupt INT0_VECTOR		
{
    u8 temp = 0;
    if(P32)
    {
        ircnt = 0;
        TR0 = 1;        
    }
    else
    {
        TR0 = 0;   
        if(20<ircnt && ircnt<60)  temp = 0;
        else if(60<ircnt && ircnt<180) temp = 1;
             else   temp = 0xff;
        
        PushByteToRxRingBuffer(&IR_RingBuffer,temp);   
    }	
}

void IR_RingBuffer_Init()
{
    RingBuffer_Init(&IR_RingBuffer,ir_rx_buff,sizeof(ir_rx_buff));
    
}

void IR_Config()
{
    Timer0_IRConfig();
    Exti0_IRConfig();
    IR_RingBuffer_Init(); 
}

u8 IRisReadyToRead()
{
    return !RingBufferIsEmpty(&IR_RingBuffer);
        
}

void IRCmd_LongPress_Count(u16 cmd)
{
    static u8 ircmd_100_cnt,ircmd_0_cnt,ircmd_4_cnt,ircmd_5_cnt,ircmd_6_cnt;
    switch(cmd)
    {
        case IR_COMMAND_100 :
        {
            ircmd_100_cnt++;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            
            if(ircmd_100_cnt>=1)   {ircmd = IR_COMMAND_100;ircmd_100_cnt = 0;}
            break;
        }
        case IR_COMMAND_0 :
        {
            ircmd_0_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            
            if(ircmd_0_cnt>=1)   {ircmd = IR_COMMAND_0;ircmd_0_cnt = 0;}
            break;
        }
        case IR_COMMAND_4 :
        {
            ircmd_4_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            
            if(ircmd_4_cnt>=1)   {ircmd = IR_COMMAND_4;ircmd_4_cnt = 0;}
            break;
        }
        case IR_COMMAND_OPEN:
        {
            ircmd_5_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            
            if(ircmd_5_cnt >=1)    {ircmd = IR_COMMAND_OPEN;ircmd_5_cnt = 0;}    
            break;
            
        }
        case IR_COMMAND_CLOSE:
        {
            ircmd_6_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            
            if(ircmd_6_cnt >=1)    {ircmd = IR_COMMAND_CLOSE;ircmd_6_cnt = 0;}
            break;
        }
         
        default:
        {
            ircmd = cmd;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            
            break;
        }
        
    }
    
  
}

void IR_Update()
{
    static u8 ircmd_bit_cnt = 0;
    static u16 ircmd_temp = 0;
    
    u8 cur;
    
    while(RingBufferIsEmpty(&IR_RingBuffer) !=1)
    {
        cur = PopByteFromRxRingBuffer(&IR_RingBuffer);
        if(cur !=1 && cur !=0)  break;
        else     cur &= 0x01;
 
        ircmd_temp <<= 1;
        ircmd_temp |= ((u16)cur & 0x0001);
        
        if(ircmd_bit_cnt == 0)
        {
            ircmd_temp &= 0x0007;
            if(ircmd_temp == 6) ircmd_bit_cnt = 3;
                
        }
        else
        {
            ircmd_temp &= 0x0FFF;
            ircmd_bit_cnt++;
            if(ircmd_bit_cnt == 12)
            {
                ircmd_bit_cnt = 0;
                IRCmd_LongPress_Count(ircmd_temp);
                ircmd_is_rcvd = 1;                
            }
        }

    }
   
}

//The end of file!









