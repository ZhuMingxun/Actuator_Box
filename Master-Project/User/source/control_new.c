#include "math.h"
#include "IORedefine.h"
#include "timer.h"
#include "delay.h"
#include "Exti.h"
#include "data_input.h"
#include "motor.h"
#include "control_new.h"
#include "data_save.h"
#include "current_output.h"
#include "phase_detect.h"
#include "GPIO.h"
#include "local.h"
#include "stdio.h"
#include "com.h"



#define TIMER1_20MS_COUNT       (20/T1_MS)
#define TIMER1_60MS_COUNT       (60/T1_MS)
#define BLINK_500MS_CNT         (500 / T1_MS)
#define TIMER1_100MS_COUNT      (100 / T1_MS)
#define TIMER1_200MS_COUNT      (200 / T1_MS)
#define TIMER1_400MS_COUNT      (400 / T1_MS)
#define TIMER1_1S_COUNT         (1000/ T1_MS) 




volatile Mode_Type mode = MODE_LOCAL;
u8 bdata Timer1_Flag;
u8  timer1_60ms_flag=0;
u16  timer1_100ms_flag=0;
u8  timer1_200ms_flag=0;
u8  timer1_400ms_flag=0;
u8  timer1_500ms_flag=0;
u8  timer1_20ms_flag=0;
u8  timer1_1s_flag=0;

void Timer1_Config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//ޡٹ֨ӥ
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ֨٤طģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ֨א׏ԅЈܶ, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//א׏ˇرՊѭ,   ENABLEܲDISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//ָ֨ʱדԴ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//ˇرˤԶٟ̙öԥ, ENABLEܲDISABLE
	TIM_InitStructure.TIM_Value     = T1_CNT;		        //
	TIM_InitStructure.TIM_Run       = ENABLE;				//ˇرԵʼۯ۳Ǵ֯֨ʱǷ, ENABLEܲDISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);
    EA = 1;    
}


/********************* Timer2_Handler************************/
void Timer1_Handler (void) interrupt TIMER1_VECTOR//Timer1 50ms
{
    static u16 timer1_20ms_cnt = 0;//50Hz
    static u16 timer1_60ms_cnt = 0;//20Hz
    static u16 timer1_100ms_cnt = 0;//10Hz
    static u16 timer1_200ms_cnt = 0;//5Hz
    static u16 timer1_400ms_cnt = 0;//2.5Hz
    static u16 timer1_500ms_cnt = 0;//2Hz 
    static u16 timer1_1s_cnt = 0;//1Hz
    
    timer1_20ms_cnt++;
    timer1_60ms_cnt++;
    timer1_100ms_cnt++;
    timer1_200ms_cnt++;
    timer1_400ms_cnt++;
    timer1_500ms_cnt++;
    timer1_1s_cnt++;
    
    //UART1_SWToDebug();
    //printf("\r\n timer1_60ms_cnt=%d",(u16)timer1_60ms_cnt);
    
    if(timer1_20ms_cnt>=(u16)TIMER1_20MS_COUNT)
    {
        timer1_20ms_flag = 1;
        timer1_20ms_cnt=0;  
        //printf("\r\n timer1_20ms_flag=%d",(int)timer1_20ms_flag);
    }
    
    if(timer1_60ms_cnt>=(u16)TIMER1_60MS_COUNT)
    {
        timer1_60ms_flag = 1;
        timer1_60ms_cnt = 0;
        //printf("\r\n timer1_60ms_flag=%d",(int)timer1_60ms_flag);
    }
    
    if(timer1_100ms_cnt>=(u16)TIMER1_100MS_COUNT)
    {
        timer1_100ms_flag = 1;
        timer1_100ms_cnt = 0;
    }
    if(timer1_200ms_cnt>=(u16)TIMER1_200MS_COUNT)
    {
        timer1_200ms_flag = 1;
        timer1_200ms_cnt = 0;
    }
    if(timer1_400ms_cnt>=(u16)TIMER1_400MS_COUNT)
    {
        timer1_400ms_cnt = 0;
        timer1_400ms_flag = 1;
    }
    if(timer1_500ms_cnt >= (u16)BLINK_500MS_CNT)//500ms
    {
        timer1_500ms_flag = 1;
        timer1_500ms_cnt = 0;
    }
    
    if(timer1_1s_cnt>= (u16)TIMER1_1S_COUNT)
    {
        timer1_1s_flag = 1;
        timer1_1s_cnt = 0;
    }
    
            
}


