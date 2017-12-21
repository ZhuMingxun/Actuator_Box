#include "mode_def.h"
#include "LCD.h"
#include "delay.h"


#define T1_MS           50  //50ms
#define T1_CNT          (65536UL - ((T1_MS*MAIN_Fosc)/12/1000))
#define TIMER1_100MS_COUNT      (100 / T1_MS)
#define TIMER1_200MS_COUNT      (200 / T1_MS)
#define TIMER1_400MS_COUNT      (400 / T1_MS)
#define TIMER1_500MS_COUNT      (500 / T1_MS)


MasterData_TypeDef  data MasterData;
MasterData_TypeDef* pMasterData = &MasterData;

u8 bdata Timer1_Flag;
sbit  timer1_50ms_flag =  Timer1_Flag^0;
sbit  timer1_100ms_flag = Timer1_Flag^1;
sbit  timer1_200ms_flag = Timer1_Flag^2;
sbit  timer1_400ms_flag = Timer1_Flag^3;
sbit  timer1_500ms_flag = Timer1_Flag^4;

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
/********************* Timer1_Handler************************/
void Timer1_Handler (void) interrupt TIMER1_VECTOR//Timer1 50ms
{
    static u8 timer1_50ms_cnt = 0;//20Hz
    static u8 timer1_100ms_cnt = 0;//10Hz
    static u8 timer1_200ms_cnt = 0;//5Hz
    static u8 timer1_400ms_cnt = 0;//2.5Hz
    static u8 timer1_500ms_cnt = 0;//2Hz 
    
    timer1_50ms_cnt++;
    timer1_100ms_cnt++;
    timer1_200ms_cnt++;
    timer1_400ms_cnt++;
    timer1_500ms_cnt++;
    
    if(timer1_50ms_cnt>=1)
    {
        timer1_50ms_flag = 1;
        timer1_50ms_cnt = 0;
    }
    if(timer1_100ms_cnt>=TIMER1_100MS_COUNT)
    {
        timer1_100ms_flag = 1;
        timer1_100ms_cnt = 0;
    }
    if(timer1_200ms_cnt>=TIMER1_200MS_COUNT)
    {
        timer1_200ms_flag = 1;
        timer1_200ms_cnt = 0;
    }
    if(timer1_400ms_cnt>=TIMER1_400MS_COUNT)
    {
        timer1_400ms_cnt = 0;
        timer1_400ms_flag = 1;
    }
    if(timer1_500ms_cnt >= TIMER1_500MS_COUNT)//500ms
    {
        timer1_500ms_flag = 1;
        timer1_500ms_cnt = 0;
    }
              
}

void MasterData_Init()
{
    pMasterData->current_input      = 18.6;
    pMasterData->opening            = 0.5;
    pMasterData->current_output     = 12.0;
    pMasterData->motor_cr           = 0;
    pMasterData->valve_position     = 2;
    pMasterData->voltage_valve      = 1.5;
    pMasterData->sen                = 50;
    pMasterData->mode               = MODE_LOCAL;
    pMasterData->motor_status       = MOTOR_STATUS_STOP;    
}

void DistantAnalog_Mode()
{
    LED_YELLOW_OFF
    while(pMasterData->mode == MODE_DISTANT_ANALOG)
    { 
        if(MasterIsReadyToRead())
        {    
            MasterData_Update();
        }
        
        if(timer1_100ms_flag)
        {
            timer1_100ms_flag = 0;
            LCD_DIS();
        }
        
        if(timer1_500ms_flag)
        {
             
            timer1_500ms_flag = 0;
            LCD_TorqueDIS_500ms();
            LED_MotorDIS_500ms();
        }
    }        
   
}

void DistantDigital_Mode()
{
    LED_YELLOW_ON
    while(pMasterData->mode == MODE_DISTANT_DIGITAL)
    { 
        if(MasterIsReadyToRead())
        {    
            MasterData_Update();
        }
        
        if(timer1_100ms_flag)
        {
            timer1_100ms_flag = 0;
            LCD_DIS();
        }
        
        if(timer1_500ms_flag)
        {
             
            timer1_500ms_flag = 0;
            LCD_TorqueDIS_500ms();
            LED_MotorDIS_500ms();
        }
    } 
}

extern u8 ircmd_is_rcvd;
void Local_Mode()
{
    LED_YELLOW_OFF
    IR_Config();
    while(pMasterData->mode == MODE_LOCAL)
    {
 
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
        if(timer1_100ms_flag)
        {
            timer1_100ms_flag = 0;
            LCD_DIS();
                           
        }
        if(timer1_500ms_flag)
        {
            timer1_500ms_flag = 0;
            LCD_TorqueDIS_500ms();
            LED_MotorDIS_500ms();
        }
        if(IRisReadyToRead())
        {
            IR_Update();
            if(ircmd_is_rcvd)
            {
                TransmitFrameToMaster();   
                ircmd_is_rcvd = 0;
            }
        }        
    }
   
}

void LackPhase_Mode()
{
    u8 ep_on = 1; 
    LED_YELLOW_OFF
    
    while(pMasterData->mode == MODE_LACK_PHASE)
    {      
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
        if(timer1_500ms_flag)
        {
            timer1_500ms_flag = 0;
            if(ep_on)
            {
                Clear_Alarm_DIS();
                ep_on = 0;
            }
            else
            {
                EP_Alarm_DIS();
                ep_on = 1;
            }   
        }  
    }            
}

void DefZero_Mode()
{
    LED_YELLOW_OFF
    Def0_DIS();
    IR_Config();
    Delay500ms();
    Delay500ms();
    while(pMasterData->mode == MODE_DEF_ZERO)
    {
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }
        
        LCD_SetDIS();
   
    }
    LCD_ClearDisplay();
    Delay500ms();
    Def0_DIS();
    Delay500ms();
    LCD_ClearDisplay();
    Delay500ms();
    Def0_DIS();
    Delay500ms(); 
}

void DefHundred_Mode()
{
    LED_YELLOW_OFF
    Def100_DIS();
    IR_Config();
    Delay500ms();
    Delay500ms();
    while(pMasterData->mode == MODE_DEF_HUNDRED)
    {
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster(); 
        }
        
        LCD_SetDIS();        
    }
    LCD_ClearDisplay();
    Delay500ms();
    Def100_DIS();
    Delay500ms();
    LCD_ClearDisplay();
    Delay500ms();
    Def100_DIS();
    Delay500ms(); 
}
void SetSen_Mode()
{
    LED_YELLOW_OFF
    LCD_ClearDisplay();
    IR_Config();
    Delay500ms();
    Delay500ms();
    
    while(pMasterData->mode == MODE_SET_SEN)
    {
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }

        if(timer1_500ms_flag)
        {
            timer1_500ms_flag = 0;
            
            Sen_DIS(pMasterData->sen);
            LCD_SetDIS();
               
        }  
    }
}

void CalOut_Mode()
{
    LED_YELLOW_OFF
    LCD_FullDisplay();
    IR_Config();
    Delay500ms();
    Delay500ms();

    while(pMasterData->mode == MODE_OUT_CAL)
    {
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }
		
        if(timer1_500ms_flag)
        {
            timer1_500ms_flag = 0;
            
            Sen_DIS(pMasterData->sen);
            LCD_SetDIS();
               
        } 
        
    }
    
}

void CalLow_Mode()
{
    LED_YELLOW_OFF
    CalLow_DIS();
    IR_Config();
    Delay500ms();
    Delay500ms();
	
	while(pMasterData->mode == MODE_CAL_LOW)
	{
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
		
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }	
	}
	
	LCD_ClearDisplay();
	Delay500ms();
	CalLow_DIS();
	Delay500ms();
	LCD_ClearDisplay();
	Delay500ms();
	CalLow_DIS();
	Delay500ms(); 
	
}

void CalHigh_Mode()
{
    LED_YELLOW_OFF
    CalHigh_DIS();
    IR_Config();
    Delay500ms();
    Delay500ms();
	
	while(pMasterData->mode == MODE_CAL_HIGH)
	{
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
		
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }
	}
	LCD_ClearDisplay();
	Delay500ms();
	
	CalHigh_DIS();
	Delay500ms();
	
	LCD_ClearDisplay();
	Delay500ms();
	
	CalHigh_DIS();
	Delay500ms(); 
}

void CalIn_Mode()
{
    LED_YELLOW_OFF
    LCD_ClearDisplay();
    IR_Config();
    Delay500ms();
    Delay500ms();
    while(pMasterData->mode == MODE_IN_CAL)
    {
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }

    }

}

void Set_Input_Low()
{
    LED_YELLOW_OFF
    Set_InputLow_DIS();
    IR_Config();
    Delay500ms();
    Delay500ms();
	while(pMasterData->mode == MODE_SETINPUT_LOW)
	{
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
		
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }
		
	}
	
	LCD_ClearDisplay();
	Delay500ms();
	
	Set_InputLow_DIS();
	Delay500ms();
	
	LCD_ClearDisplay();
	Delay500ms();
	
	Set_InputLow_DIS();
	Delay500ms(); 
}

void Set_Input_High()
{
    LED_YELLOW_OFF
	Set_InputHigh_DIS();
    IR_Config();
    Delay500ms();
    Delay500ms();
	while(pMasterData->mode == MODE_SETINPUT_HIGH)
	{
        if(MasterIsReadyToRead())
        { 
            MasterData_Update();
        }
		
        if(IRisReadyToRead())
        {
            IR_Update();
            TransmitFrameToMaster();
        }
		
	}
	
	LCD_ClearDisplay();
	Delay500ms();
	
	Set_InputHigh_DIS();
	Delay500ms();
	
	LCD_ClearDisplay();
	Delay500ms();
	
	Set_InputHigh_DIS();
	Delay500ms(); 
	
}





