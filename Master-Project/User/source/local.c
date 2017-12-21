#include "math.h"
#include <stdio.h>
#include "IORedefine.h"
#include "timer.h"
#include "delay.h"
#include "Exti.h"
#include "data_input.h"
#include "motor.h"
#include "control_new.h"
#include "mode_detect.h"
#include "data_save.h"
#include "current_output.h"
#include "phase_detect.h"
#include "distant_analog.h"
#include "local.h"
#include "ringbuffer.h"
#include "com.h"
#include "actuator_config.h"

/* 红外命令宏定义 */
#define IR_COMMAND_100      0x0d04
#define IR_COMMAND_0        0x0d02
#define IR_COMMAND_OPEN     0x0d20
#define IR_COMMAND_CLOSE    0x0d10
#define IR_COMMAND_STOP     0x0c84
#define IR_COMMAND_4        0x0d08//3336
#define IR_COMMAND_0_100    (IR_COMMAND_0 | IR_COMMAND_100)
#define IR_COMMAND_4_0      (IR_COMMAND_4 | IR_COMMAND_0)
#define IR_COMMAND_4_100    (IR_COMMAND_4 | IR_COMMAND_100)   
#define IR_COMMAND_4_OPEN   (IR_COMMAND_4 | IR_COMMAND_OPEN)//3368
#define IR_COMMAND_4_CLOSE  (IR_COMMAND_4 | IR_COMMAND_CLOSE)//3352

#define SEN0_05MA   160
#define SEN0_1MA    320
#define SEN0_15MA   480
#define SEN0_2MA    640
#define SEN0_25MA   800
#define SEN0_3MA    960
#define SEN0_35MA   1120
#define SEN0_4MA    1280
#define SEN0_45MA   1440
#define SEN0_5MA    1600



#define LOCAL_LOCK_DELAY    40//60ms*4=2.4s
#define LOCAL_OPEN_ON       0
#define LOCAL_CLOSE_ON      0



 u8 local_stat;
 u8 local_stat_cnt;

volatile LocalCmd_Type local_cmd = LOCAL_CMD_STOP;
//static u8 local_lock_cnt = 0;
//static u8 local_lock_flag = 0;
//static u8 local_lock_startflag = 0;

extern u8 ircmd_decode_ok;
u16 ircmd;

u16 code  sen_table[10] = {SEN0_05MA,SEN0_1MA,SEN0_15MA,SEN0_2MA,SEN0_25MA,SEN0_3MA,SEN0_35MA,SEN0_4MA,SEN0_45MA,SEN0_5MA};
const u16* p_sen = &sen_table[0];

//void LocalCmdLock_Detect();
void LocalDef_Detect();
static void IRCmd_To_LocalCmd();
static void Local_Stat_Detect();
static void Local_Stat_Count_per60ms();

//void Exti23_Config()
//{
//    EXTI_InitTypeDef EXTI_InitStructure;
//    EXTI_InitStructure.EXTI_Interrupt = ENABLE;
//    Ext_Inilize(EXT_INT2,&EXTI_InitStructure);
//    Ext_Inilize(EXT_INT3,&EXTI_InitStructure);
//     
//}

void Exti23_Disable()
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Interrupt = DISABLE;
    Ext_Inilize(EXT_INT2,&EXTI_InitStructure);
    Ext_Inilize(EXT_INT3,&EXTI_InitStructure);   
    
}

void LocalMode_Init()
{
    local_cmd = LOCAL_CMD_STOP;
    local_stat = LOCAL_STAT_STOP;
    local_stat_cnt=0;
    Motor_Stop();
    ReadSystemPara();
    Acquire_Data();
    PWM_Update();
    //
    //printf("\r\n Local Mode ==============================\r\n");
    
}


u8 KeyScan(u8 key)
{
	u8 reval = 0;
	
	switch(key)
	{
		case KEY_LOCAL:
		{
			if(LOCAL == LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL == LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
					reval = 1;
				
			}
			break;
		}
		
		case KEY_STOP:
		{
			if(LOCAL_STOP == LOCAL_STOP_ON && LOCAL != LOCAL_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL_STOP == LOCAL_STOP_ON && LOCAL != LOCAL_ON)
					reval = 1;
				
			}
			break;
		}
		
		case KEY_REMOTE:
		{
			if(LOCAL != LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
			{
				delay_ms(SCAN_RMTDELAY_MS);
				if(LOCAL != LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
					reval = 1;
				
			}
			
			break;
		}
		
		case KEY_OPEN:
		{
			if(LOCAL_OPEN == LOCAL_OPEN_ON && LOCAL_CLOSE != LOCAL_CLOSE_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL_OPEN == LOCAL_OPEN_ON && LOCAL_CLOSE != LOCAL_CLOSE_ON)
					reval = 1;
				
			}
			break;
		}
		
		case KEY_CLOSE:
		{
			if(LOCAL_CLOSE == LOCAL_CLOSE_ON && LOCAL_OPEN != LOCAL_OPEN_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL_CLOSE == LOCAL_CLOSE_ON && LOCAL_OPEN != LOCAL_OPEN_ON)
					reval = 1;
				
			}
			break;	
		}	
		
		default:break;

	}
	return reval;
	
	
}

extern u16 remove_jumpdat_val;

void Local_Mode()
{ 
    LocalMode_Init();
    
    while( mode == MODE_LOCAL )
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        
        #ifdef TRAVEL_PROTECT_MACHINE
            TravelProtect_Machine();
        #else
            TravelProtect();
        #endif
        
        
        
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
            
            if(ircmd_decode_ok)
            {
                ircmd_decode_ok = 0;
                IRCmd_To_LocalCmd();
            }
			if(mode != MODE_LOCAL)
				break;
        }
		
		Local_Stat_Detect();
        
		Local_Control();
        
        if(timer1_20ms_flag==1)
        {
            timer1_20ms_flag = 0;
            Acquire_Data();
            Update_InputCurrent();
            //UART1_SWToDebug();
            //printf("\r\n%d,%d,%f,%d,%d",pData_ADC->adcvalue_valve,remove_jumpdat_val,pData_ADC->adcvalue_valve_filtered,pSystemParam->adcvalue_valvelow,pSystemParam->adcvalue_valvehigh);
            //printf("\r\n local_stat=%d  local_cmd=%d",(int)local_stat,(int)local_cmd);
			//printf("\r\n Local Mode ");

        }
         
        if(timer1_60ms_flag == 1)
        {
            timer1_60ms_flag = 0;
            MotorStop_Delay_60ms();
			

        }
        
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
                        
            Update_InputCurrent();
            LimitPosition_Output();
            Torque_Detect();
            PWM_Update();
            TransmitFrameToSlaver();//发送主机数据
        }

        if(timer1_200ms_flag == 1)
        {
            timer1_200ms_flag = 0;
            SystemMode_Detect();
            if(mode == MODE_DISTANT_ANALOG || mode == MODE_DISTANT_DIGITAL) break;
            
            LocalDef_Detect();
            if(mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO || mode==MODE_CAL_LOW || mode==MODE_CAL_HIGH)   break;
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 
      
        }
        
        if(timer1_500ms_flag == 1)
        {
            timer1_500ms_flag = 0;
            IntegAlarm();
        }
        
        if(timer1_1s_flag == 1)
        {
            timer1_1s_flag = 0;
            MotorErr_Detect();
            Motor_PosRev();
            Local_Stat_Count_per60ms();
            //UART1_SWToDebug();
            //printf("\r\n local_stat_cnt=%d",(int)local_stat_cnt);
            
        } 
  
    }
  
}
static void IRCmd_To_LocalCmd()
{
    switch(ircmd)
    {
        case IR_COMMAND_100:    break;   
        case IR_COMMAND_0:      break;       
        case IR_COMMAND_OPEN:   
        {
            local_cmd = LOCAL_CMD_OPEN;
            local_stat = LOCAL_STAT_OPEN_KEEP;
            break;
        }    
        case IR_COMMAND_CLOSE:  
        {
            local_cmd = LOCAL_CMD_CLOSE;
            local_stat = LOCAL_STAT_CLOSE_KEEP;
            
            break;
        } 
        case IR_COMMAND_STOP:   
        {
            local_cmd = LOCAL_CMD_STOP; 
            local_stat = LOCAL_STAT_STOP;
            break;
        }
        case IR_COMMAND_4:      break;
        case IR_COMMAND_0_100:
        {
            if(LOCAL_STOP == LOCAL_STOP_ON) 
            mode =  MODE_SET_SEN; 
            break;
        }
        case IR_COMMAND_4_0:    
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_DEF_ZERO;
            break;
        }
        case IR_COMMAND_4_100:  
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_DEF_HUNDRED;
            break;
        }
        case IR_COMMAND_4_OPEN:
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_CAL_HIGH;
            break;
        }
        case IR_COMMAND_4_CLOSE:
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_CAL_LOW;
            break;
        }
        default:break;  
    }
							//UART1_SWToDebug();
					//printf("\r\n ircmd=%d",(int)ircmd);
    ircmd = 0;

}

/**************现场控制函数******************/
void Local_Control()
{
    
    if(KeyScan(KEY_STOP) )
    {
        local_cmd = LOCAL_CMD_STOP;
    }   
    switch(local_cmd)
    {
        case LOCAL_CMD_STOP:
        {
            Motor_Stop();
            break;
        }
        
        case LOCAL_CMD_OPEN:
        {
            MotorOpen_Control();
            break;
        }
        
        case LOCAL_CMD_CLOSE:
        {
            MotorClose_Control(); 
            break;
        }
        default:break;
    }
    
                 
}

//===========================================================================================
//===================================标定函数================================================
//===========================================================================================
signed char zero_flag = -1;//编码器过零标志,-1不过零，+1过零

void CloseDir_Zero_Detect()
{
	static u16 value_pre;
	static int delta1,delta2,delta3;
	
	//判断关方向
	switch(local_cmd)
	{
		case LOCAL_CMD_STOP:
		{
			delta1 = 0;
			delta2 = 0;
			delta3 = 0;
			break;
		}
		
		case LOCAL_CMD_CLOSE:
		{
			delta1 = delta2;
			delta2 = delta3;
			delta3 = pData_ADC->adcvalue_encoder - value_pre;
			
			if(delta1>0 && delta2>0 && delta3>0)
			{
				pSystemParam->close_dir = 1;//关向递增
			}
			if(delta1<0 && delta2<0 && delta3<0)
			{
				pSystemParam->close_dir = -1;//关向递减
			}	
			
			break;
		}
		
		case LOCAL_CMD_OPEN:
		{
			delta1 = delta2;
			delta2 = delta3;
			delta3 = pData_ADC->adcvalue_encoder - value_pre;
			
			if(delta1>0 && delta2>0 && delta3>0)
			{
				pSystemParam->close_dir = -1;//关向递增
			}
			if(delta1<0 && delta2<0 && delta3<0)
			{
				pSystemParam->close_dir = 1;//关向递减
			}	
			break;
		}
		default:
		{
			delta1 = 0;
			delta2 = 0;
			delta3 = 0;
			break;
		}
	}
	
	//判断行程过零
	if((int)(pSystemParam->adcvalue_valvelow - pSystemParam->adcvalue_valvehigh)*(pSystemParam->close_dir)>0)
		zero_flag = -1;
	if((int)(pSystemParam->adcvalue_valvelow - pSystemParam->adcvalue_valvehigh)*(pSystemParam->close_dir)<0)
		zero_flag = 1;

	value_pre = pData_ADC->adcvalue_encoder;
	
}


#define DEF_STEP0_STOP	0
#define DEF_STEP1_LOCAL	1
#define DEF_STEP2_STOP	2
#define DEF_STEP3_LOCAL	3

void DefZero_Mode()
{
	u8 def_step = 0;
    u16 u16_data_tmp = 0;
    u8 exit_cnt = 15;
	u8 cal_low_cnt = 0;
    u8 ir_4_cnt=0;
    u8 ir_0_cnt=0;

    Motor_CR &= ~(0x03);
	//UART1_SWToDebug();
	//printf("\r\n DEF_STEP0_STOP");
    
    while(mode == MODE_DEF_ZERO)
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
        }
        
        switch(ircmd)
        {
            case IR_COMMAND_OPEN:   local_cmd = LOCAL_CMD_OPEN;ircmd = 0;break;
            case IR_COMMAND_CLOSE:  local_cmd = LOCAL_CMD_CLOSE;ircmd = 0;break;  
            case IR_COMMAND_STOP:   local_cmd = LOCAL_CMD_STOP;ircmd = 0;break;
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    local_cmd = LOCAL_CMD_STOP;
                    mode = MODE_LOCAL;
                }
                
                ircmd = 0;

                break;
            }                        
            case IR_COMMAND_0:
            {
                ir_0_cnt++;
                if(ir_0_cnt>4)
                {
                    ir_0_cnt=0;
                    if(local_cmd == LOCAL_CMD_STOP)
                    {
                        if(use_encoder_flag)
                        {
                            pSystemParam->adcvalue_valvelow = pData_ADC->adcvalue_encoder;
                        }
                        else
                        {
                            pSystemParam->adcvalue_valvelow = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                        }
                        IapWrite_Opening0_adc(pSystemParam->adcvalue_valvelow);
                        IapWrite_CloseDir(pSystemParam->close_dir);
                        mode = MODE_LOCAL;
                    }
                }
                ircmd = 0;
                break;
            }
            default:break;  
        }
		
        if(timer1_20ms_flag==1)
        {
            timer1_20ms_flag = 0;
            Acquire_Data();
			CloseDir_Zero_Detect();
        }
		
        if(timer1_60ms_flag == 1)
        {
            timer1_60ms_flag = 0;
            MotorStop_Delay_60ms();
			Local_Stat_Count_per60ms();
            //LocalCmdLock_Detect();
        }
		
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
            Torque_Detect();
            TravelProtect_Def();
            TransmitFrameToSlaver();   
            
        }
		
        if(timer1_200ms_flag == 1)
        {
            timer1_200ms_flag = 0;
			
			if(def_step==DEF_STEP2_STOP)
			{
				if(exit_cnt >0) exit_cnt--;
			}
			
			if(def_step==DEF_STEP0_STOP)
			{
				if(KeyScan(KEY_CLOSE))
				{
					cal_low_cnt++;
					if(cal_low_cnt>30)
					{
						mode = MODE_CAL_LOW;
						cal_low_cnt = 0;
						return;
					}
				}
				else
				{
					cal_low_cnt = 0;
				}
			}
			
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 
        }
		

		
		switch(def_step)
		{
			case DEF_STEP0_STOP:
			{
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP1_LOCAL;
					//UART1_SWToDebug();
					//printf("\r\n Switch to DEF_STEP1_STOP");
				}
				break;
			}
			
			case DEF_STEP1_LOCAL:
			{
				if(KeyScan(KEY_STOP))
				{

					def_step = DEF_STEP2_STOP;
					exit_cnt = 15;
					//UART1_SWToDebug();
					//printf("\r\n Switch to DEF_STEP2_STOP");
				}
				break;
			}
			
			case DEF_STEP2_STOP:
			{
				
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP3_LOCAL;
					//UART1_SWToDebug();
					//printf("\r\n Switch to DEF_STEP3_STOP");
				}
				
				break;
			}
			
			case DEF_STEP3_LOCAL:
			{
				if(exit_cnt>0)
				{
                    if(use_encoder_flag)
                    {
                        pSystemParam->adcvalue_valvelow = pData_ADC->adcvalue_encoder;
                    }
                    else
                    {
                        pSystemParam->adcvalue_valvelow = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                    }
                    IapWrite_Opening0_adc(pSystemParam->adcvalue_valvelow);
					IapWrite_CloseDir(pSystemParam->close_dir);
                    
				}
					//UART1_SWToDebug();
					//printf("\r\n DEF_STEP3_LOCAL Exit!");
				mode = MODE_LOCAL; 
				
				break;
			}
			default:break;
			
		}
		Local_Stat_Detect();
		Local_Control();

        
    }
    
}

void DefHundred_Mode()
{
	u8 def_step = 0;
    u16 u16_data_tmp = 0;
    static u8 exit_cnt = 15;
	u8 cal_high_mode_cnt = 0;
    u8 ir_4_cnt = 0;
    u8 ir_100_cnt = 0;
    Motor_CR &= ~(0x03);

    
    while(mode == MODE_DEF_HUNDRED)
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
		
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
        }

        switch(ircmd)
        {
            case IR_COMMAND_OPEN:   local_cmd = LOCAL_CMD_OPEN;ircmd = 0;break;
            case IR_COMMAND_CLOSE:  local_cmd = LOCAL_CMD_CLOSE;ircmd = 0;break;  
            case IR_COMMAND_STOP:   local_cmd = LOCAL_CMD_STOP;ircmd = 0;break;
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    
                    local_cmd = LOCAL_CMD_STOP;
                    mode = MODE_LOCAL;
                }
                ircmd = 0;
                break;
            } 
            case IR_COMMAND_100:
            {
                ir_100_cnt++;
                if(ir_100_cnt>4)
                {
                    ir_100_cnt=0;
                    if(local_cmd == LOCAL_CMD_STOP)
                    {
                        if(use_encoder_flag)
                        {
                            pSystemParam->adcvalue_valvehigh = pData_ADC->adcvalue_encoder;
                        }
                        else
                        {
                            pSystemParam->adcvalue_valvehigh = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                        }
                        IapWrite_Opening100_adc(pSystemParam->adcvalue_valvehigh);
                        IapWrite_CloseDir(pSystemParam->close_dir);
                        mode = MODE_LOCAL;
                    }
                }
                ircmd = 0;
                break;
            }
            default:break;  
        }
		
        if(timer1_20ms_flag==1)
        {
            timer1_20ms_flag = 0;
            Acquire_Data();
			CloseDir_Zero_Detect();
        }
        if(timer1_60ms_flag == 1)
        {
            timer1_60ms_flag = 0;
            MotorStop_Delay_60ms();
			Local_Stat_Count_per60ms();
            //LocalCmdLock_Detect();
        }
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
            Torque_Detect();
            TravelProtect_Def();
            TransmitFrameToSlaver();   
            
        }
        if(timer1_200ms_flag == 1)
        {
            timer1_200ms_flag = 0;
			
			if(def_step==DEF_STEP2_STOP)
			{
				if(exit_cnt >0) exit_cnt--;
			}
			
			if(def_step==DEF_STEP0_STOP)
			{
				if(KeyScan(KEY_OPEN))
				{
					cal_high_mode_cnt++;
					if(cal_high_mode_cnt>30)
					{
						mode = MODE_CAL_HIGH;
						cal_high_mode_cnt = 0;
						return;
					}
	//		    UART1_SWToDebug();
	//			printf("\r\ncal_hight_cnt:%d ",(int)cal_high_mode_cnt);
				}
				else
				{
					cal_high_mode_cnt = 0;
				}
			}
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 

			
        }
		
		switch(def_step)
		{
			
			case DEF_STEP0_STOP:
			{
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP1_LOCAL;
				}
				break;
			}
			
			case DEF_STEP1_LOCAL:
			{
				if(KeyScan(KEY_STOP))
				{
				    def_step = DEF_STEP2_STOP;
					exit_cnt = 15;
				}
				break;
			}
			
			case DEF_STEP2_STOP:
			{
				
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP3_LOCAL;
					//delay_ms(1000);
				}
				
				break;
			}
			
			case DEF_STEP3_LOCAL:
			{
				if(exit_cnt>0)
				{
                    if(use_encoder_flag)
                    {
                        pSystemParam->adcvalue_valvehigh = pData_ADC->adcvalue_encoder;
                    }
                    else
                    {
                        pSystemParam->adcvalue_valvehigh = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                    }
                    IapWrite_Opening100_adc(pSystemParam->adcvalue_valvehigh);
					IapWrite_CloseDir(pSystemParam->close_dir);
                    
				}

				mode = MODE_LOCAL; 
				
				break;
			}
			default:break;
			
		}
		Local_Stat_Detect();
		Local_Control();
        
    }    
}

void SetSen_Mode()
{
    u8 i;
    u8 ir_op_cnt=0;
    u8 ir_cl_cnt=0;
    u8 ir_4_cnt=0;    
    pSystemParam->sen = IapRead_Sen();
    if(pSystemParam->sen == 0xff) pSystemParam->sen = SEN0_5MA;

    p_sen = &sen_table[0];
    for(i=0;i<10;i++)
    {
        if(pSystemParam->sen == *p_sen)   break;
        else
        {
            if(i == 9)
            {
                p_sen = &sen_table[0];
            }
            else    p_sen++;
        }
        
    }
//	ircmd = 0;
    
    while(mode == MODE_SET_SEN)
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
        }


        switch(ircmd)
        {
            case IR_COMMAND_OPEN:
            {
                ir_op_cnt++;
                if(ir_op_cnt>4)
                {
                    ir_op_cnt=0;
                    if(*p_sen == sen_table[9])  p_sen = &sen_table[0];
                    else    p_sen++;
                    
                    pSystemParam->sen = *p_sen;
                    IapWrite_Sen(pSystemParam->sen);
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                ir_cl_cnt++;
                if(ir_cl_cnt>4)
                {
                    ir_cl_cnt=0;
                    if(*p_sen == sen_table[0])  p_sen = sen_table+9;
                    else    p_sen--;

                    pSystemParam->sen = *p_sen;
                    IapWrite_Sen(pSystemParam->sen);
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    mode = MODE_LOCAL;
                }
                ircmd = 0;
                break; 
            }
                
            default:break;    

        }
        
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
            TransmitFrameToSlaver();      
        }

    }
      
}



//低端校准4mA
void CalLow_Mode()
{

    u16 pwm_cal_cnt;
	u8 ir_op_cnt=0;
	u8 ir_cl_cnt=0;
	u8 ir_4_cnt=0;
	u8 ir_0_cnt=0;
	//pSystemParam->cal_low = IapRead_CalLow();
	pwm_cal_cnt = pSystemParam->cal_low;
	
    Acquire_Data();
    PWM_Cal(pwm_cal_cnt);
    while(KeyScan(KEY_CLOSE))
	{
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif  
		TransmitFrameToSlaver(); 
		delay_ms(50);
	}
    while(mode == MODE_CAL_LOW)
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif    
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
        }
        if(timer1_20ms_flag==1)
        {
            timer1_20ms_flag = 0;
            Acquire_Data();
        }
        
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
            TransmitFrameToSlaver();  
			//旋钮检测
			if(KeyScan(KEY_OPEN))
			{
				pwm_cal_cnt++;
			}
			if(KeyScan(KEY_CLOSE))
			{
				pwm_cal_cnt--;
			}			
        }
        
        if(timer1_1s_flag)
        {
            timer1_1s_flag = 0;
            if(KeyScan(KEY_LOCAL))//确定
            {
                pSystemParam->cal_low = pwm_cal_cnt;
                IapWrite_CalLow(pwm_cal_cnt);
                mode = MODE_LOCAL;
                break;
            }
            if(KeyScan(KEY_REMOTE))//退出
            {
                mode = MODE_DISTANT_ANALOG;
                break;	
            }
        }

		PWM_Cal(pwm_cal_cnt);
		


        switch(ircmd)
        {
            case IR_COMMAND_OPEN:
            {
                ir_op_cnt++;
                if(ir_op_cnt>2)
                {
                    ir_op_cnt=0;
                    pwm_cal_cnt += 1;
                    PWM_Cal(pwm_cal_cnt);
                    
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                ir_cl_cnt++;
                if(ir_cl_cnt>2)
                {
                    ir_cl_cnt=0;
                    pwm_cal_cnt -= 1;
                    PWM_Cal(pwm_cal_cnt);
                }
                ircmd = 0;
                break;  
            }
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    mode = MODE_LOCAL;
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_0:
            {
                ir_0_cnt++;
                if(ir_0_cnt>4)
                {
                    ir_0_cnt = 0;
                    pSystemParam->cal_low = pwm_cal_cnt;
                    IapWrite_CalLow(pwm_cal_cnt);
                    mode = MODE_LOCAL;
                }
                ircmd = 0;
                break;
            }
            
            default:break; 
        }

    }	
}

//高端校准20mA
void CalHigh_Mode()
{

    u16 pwm_cal_cnt;
	u16 rd_cnt;
	u8 ir_op_cnt=0;
	u8 ir_cl_cnt=0;
	u8 ir_4_cnt=0;
	u8 ir_100_cnt=0;
	pSystemParam->cal_high = IapRead_CalHigh();
	pwm_cal_cnt = pSystemParam->cal_high;
	
    Acquire_Data();
	PWM_Cal(pwm_cal_cnt);

	
	while(KeyScan(KEY_OPEN))
	{
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif 
		TransmitFrameToSlaver(); 
		delay_ms(50);
	}
	
	//UART1_SWToDebug();
	//printf("\r\npwm_cal_cnt:%d ",(int)pwm_cal_cnt);
	
    while(mode == MODE_CAL_HIGH)
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif    
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
        }
        if(timer1_20ms_flag==1)
        {
            timer1_20ms_flag = 0;
            Acquire_Data();
        }
        
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
            TransmitFrameToSlaver();    
			if(KeyScan(KEY_OPEN))
			{
				pwm_cal_cnt++;
			}
			if(KeyScan(KEY_CLOSE))
			{
				pwm_cal_cnt--;
			}				
        }

        if(timer1_1s_flag)
        {
            timer1_1s_flag = 0;
            if(KeyScan(KEY_LOCAL))//确定
            {
                pSystemParam->cal_high = pwm_cal_cnt;
                IapWrite_CalHigh(pwm_cal_cnt);
                rd_cnt = IapRead_CalHigh();
                //UART1_SWToDebug();
                //printf("\r\nset_cnt:%d ",(int)pwm_cal_cnt);
                //printf("\r\nread_cnt:%d ",(int)rd_cnt);
                //printf("\r\ncal_high:%d ",(int)pSystemParam->cal_high);
                
                mode = MODE_LOCAL;
                break;
            }
            if(KeyScan(KEY_REMOTE))//退出
            {
                mode = MODE_DISTANT_ANALOG;
                break;	
            }
        }            
		
		PWM_Cal(pwm_cal_cnt);

        switch(ircmd)
        {
            case IR_COMMAND_OPEN:
            {
                ir_op_cnt++;
                if(ir_op_cnt>2)
                {
                    ir_op_cnt=0;
                    pwm_cal_cnt += 1;
                    PWM_Cal(pwm_cal_cnt);
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                ir_cl_cnt++;
                if(ir_cl_cnt>2)
                {
                    ir_cl_cnt=0;
                    pwm_cal_cnt -= 1;
                    PWM_Cal(pwm_cal_cnt);
                }
                ircmd = 0;
                break;  
            }
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    mode = MODE_LOCAL;
                       
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_100:
            {
                ir_100_cnt++;
                if(ir_100_cnt>4)
                {
                    ir_100_cnt = 0;
                    pSystemParam->cal_high = pwm_cal_cnt;
                    IapWrite_CalHigh(pwm_cal_cnt);
                    mode = MODE_LOCAL;
                }
                ircmd = 0; 
                break;                
            }
            
            
            default:break; 
        }

    }	
}


void SetInputLow_Mode()
{
	u8 samp_cnt = 0;
	u16 read_value;
	while(mode==MODE_SETINPUT_LOW)
	{
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
        }
        
        if(timer1_20ms_flag==1)
        {
            timer1_20ms_flag = 0;
			samp_cnt++;
            Update_InputCurrent();
			
			if(samp_cnt>50)
			{
				samp_cnt = samp_cnt;
				pSystemParam->setinput_low = (u16)pData_ADC->adcvalue_input_filtered;
				IapWrite_SetInputLow(pSystemParam->setinput_low);
				mode = MODE_DISTANT_ANALOG;
				
				read_value = IapRead_SetInputLow();
				//UART1_SWToDebug();
				//printf("\r\n setinput_low:%d",pSystemParam->setinput_low);
				//printf("\r\n read_value_low:%d",read_value);
				
			}
        }
		
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
            TransmitFrameToSlaver();    
        } 
	
	}
	
}

void SetInputHigh_Mode()
{
	u8 samp_cnt = 0;
	u16 read_value;
	
	while(mode==MODE_SETINPUT_HIGH)
	{
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        
        if(SlaverIsReadyToRead())
        {
            SlaverData_Update();//接收从机数据
        }
        
        if(timer1_20ms_flag==1)
        {
            timer1_20ms_flag = 0;
			samp_cnt++;
            Update_InputCurrent();
			
			if(samp_cnt>50)
			{
				samp_cnt = samp_cnt;
				pSystemParam->setinput_high = (u16)pData_ADC->adcvalue_input_filtered;
				IapWrite_SetInputHigh(pSystemParam->setinput_high);
				mode = MODE_DISTANT_ANALOG;	
				read_value = IapRead_SetInputHigh();
				
				//UART1_SWToDebug();
				//printf("\r\n setinput_high:%d",pSystemParam->setinput_high);
				//printf("\r\n read_value_high:%d",read_value);
			}
        }
		
        if(timer1_100ms_flag == 1)
        {
            timer1_100ms_flag = 0;
            TransmitFrameToSlaver();   
            
        } 

		
	}	
}



static void LocalDef_Detect()//200ms 执行一次
{
	static u8 open_cnt;//6s
	static u8 close_cnt;

    if(KeyScan(KEY_STOP) && mode == MODE_LOCAL)
    {
        if(KeyScan(KEY_OPEN))
        {
			open_cnt++;
			if(open_cnt>15)
			{
				mode = MODE_DEF_HUNDRED;
				open_cnt = 0;
				
			}
				
			
        }
        else if(KeyScan(KEY_CLOSE))
        {
			close_cnt++;
			if(close_cnt>15)
			{
				mode = MODE_DEF_ZERO;
				close_cnt = 0;
			}
        }
		else
		{
			open_cnt = 0;
			close_cnt = 0;
		}

        
    }
    
}


extern bit motordelay_flag;
static void Local_Stat_Detect()
{

	switch(local_stat)
	{
		case LOCAL_STAT_STOP://现场停止状态
		{
			local_stat_cnt = 0;
			
			if(KeyScan(KEY_LOCAL))
			{     
				
                if(KeyScan(KEY_OPEN) && motordelay_flag)
				{
					local_stat = LOCAL_STAT_OPEN_INCHING;
				}
				else if(KeyScan(KEY_CLOSE) && motordelay_flag)
				{
					local_stat = LOCAL_STAT_CLOSE_INCHING;
					
				}
			}
			
			break;
		}
		
		case LOCAL_STAT_OPEN_INCHING://开-点动区
		{
			if(KeyScan(KEY_STOP) || !KeyScan(KEY_OPEN))
			{
				local_stat = LOCAL_STAT_STOP;
				break;
			}
			
			if(local_stat_cnt>2)//持续超3秒
			{
				//local_stat_cnt = 0;
				local_stat = LOCAL_STAT_OPEN_KEEP;
			}
			break;

		}
		
		case LOCAL_STAT_CLOSE_INCHING://关-点动区
		{
			if(KeyScan(KEY_STOP) || !KeyScan(KEY_CLOSE))
			{
				local_stat = LOCAL_STAT_STOP;
				break;
			}
			
			if(local_stat_cnt>2)//持续超3秒
			{
				//local_stat_cnt = 0;
				local_stat = LOCAL_STAT_CLOSE_KEEP;
			}
			break;

		}
		
		case LOCAL_STAT_OPEN_KEEP:
		{
			if(KeyScan(KEY_STOP) || KeyScan(KEY_CLOSE))
			{
				local_stat = LOCAL_STAT_STOP;
			}
			break;

		}
		
		case LOCAL_STAT_CLOSE_KEEP:
		{
			if(KeyScan(KEY_STOP) || KeyScan(KEY_OPEN))
			{
				local_stat = LOCAL_STAT_STOP;
			}
			break;

		}
		default:
		{
			local_stat = LOCAL_STAT_STOP;
			break;
		}
		
	}
	
	switch(local_stat)
	{
		case LOCAL_STAT_STOP:
		{
			local_cmd = LOCAL_CMD_STOP;
			break;
		}
		case LOCAL_STAT_OPEN_INCHING:case LOCAL_STAT_OPEN_KEEP:
		{
			local_cmd = LOCAL_CMD_OPEN;
			break;
		}
		case LOCAL_STAT_CLOSE_INCHING:case LOCAL_STAT_CLOSE_KEEP:
		{
			local_cmd = LOCAL_CMD_CLOSE;
			break;
		}
		default:
		{
			local_cmd = LOCAL_CMD_STOP;
			break;
		}
		
	}	
}




static void Local_Stat_Count_per60ms()
{
	if(local_stat==LOCAL_STAT_OPEN_INCHING || local_stat==LOCAL_STAT_CLOSE_INCHING)
	{
		local_stat_cnt++;
	}	
}

/********************* INT2中断服务*************************/
void Ext_INT2 (void) interrupt INT2_VECTOR	// LOCAL_OPEN
{
//    delay_ms(25); 
//    if(LOCAL_OPEN == LOCAL_OPEN_ON)
//    {
//        if(LOCAL == LOCAL_ON || mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO)
//        {
//            local_cmd = LOCAL_CMD_OPEN;
//            local_lock_cnt = 0;
//            local_lock_flag = 0;
//            local_lock_startflag = 1;
//        }
//          
//    }

}

/********************* INT3中断服务*************************/
void Ext_INT3 (void) interrupt INT3_VECTOR   // LOCAL_CLOSE
{
//    delay_ms(25);
//    if(LOCAL_CLOSE == LOCAL_CLOSE_ON)
//    {  
//        if(LOCAL == LOCAL_ON || mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO)
//        {
//            local_cmd = LOCAL_CMD_CLOSE;
//            local_lock_cnt = 0;
//            local_lock_flag = 0;
//            local_lock_startflag = 1;
//        }
//    }

}

//void CalOut_Mode()
//{
//    u16 pwm_cal_cnt;
//    Acquire_Data();
//	pwm_cal_cnt = (u16)(0.004f*pSystemParam->res_output*2500.0f/5.0f);

//    PWM_Cal(pwm_cal_cnt);
//    
//    while(mode == MODE_OUT_CAL)
//    {
//        #ifdef WATCH_DOG
//        WDT_CONTR = WATCH_DOG_RSTVAL;
//        #endif    
//        if(SlaverIsReadyToRead())
//        {
//            SlaverData_Update();//接收从机数据
//        }
//        if(timer1_20ms_flag==1)
//        {
//            timer1_20ms_flag = 0;
//            Acquire_Data();
//        }
//        
//        if(timer1_100ms_flag == 1)
//        {
//            timer1_100ms_flag = 0;
//            TransmitFrameToSlaver();   
//            
//        }
//		
//        switch(ircmd)
//        {
//            case IR_COMMAND_OPEN:
//            {
//                pwm_cal_cnt += 2;
//                PWM_Cal(pwm_cal_cnt);
//                ircmd = 0;
//                break;
//            }
//            case IR_COMMAND_CLOSE:
//            {
//                pwm_cal_cnt -= 2;
//                PWM_Cal(pwm_cal_cnt);
//                ircmd = 0;
//                break;  
//            }
//            case IR_COMMAND_4:
//            {
//                pSystemParam->res_output = (pData_Voltage->voltage_vcc * (float)pwm_cal_cnt) / (0.02f * 2500.0f);
//                IapWrite_R_out(pSystemParam->res_output); 
//                mode = MODE_LOCAL;
//                ircmd = 0;
//                break;
//            }
//            default:break; 
//        }

//    }
//  
//}
//void CalIn_Mode()
//{
//    
//    while(mode == MODE_IN_CAL)
//    {
//        #ifdef WATCH_DOG
//        WDT_CONTR = WATCH_DOG_RSTVAL;
//        #endif
//        
//        if(SlaverIsReadyToRead())
//        {
//            SlaverData_Update();//接收从机数据
//        }
//        
//        if(timer1_20ms_flag==1)
//        {
//            timer1_20ms_flag = 0;
//            Acquire_Data();
//        }
//        
//        if(timer1_100ms_flag == 1)
//        {
//            timer1_100ms_flag = 0;
//            TransmitFrameToSlaver();   
//            
//        } 
//        switch(ircmd)
//        {
//            case IR_COMMAND_4:
//            {
//                pSystemParam->res_input = pData_Voltage->voltage_input / 0.02;
//                IapWrite_R_in(pSystemParam->res_input);
//                mode = MODE_LOCAL;
//                break;
//            }
//            case IR_COMMAND_CLOSE:
//            {
//                mode = MODE_LOCAL;
//                break;                  
//            }
//            default:break;               
//            
//        }
//    }  
//}
//static void LocalCmdLock_Detect()//执行周期 60ms
//{
//    if(local_lock_startflag == 1)
//    {
//        if(LOCAL_OPEN == LOCAL_OPEN_ON || LOCAL_CLOSE == LOCAL_CLOSE_ON)
//        {
//            if(local_lock_cnt<LOCAL_LOCK_DELAY) local_lock_cnt++;
//            else
//            {
//                if(local_lock_flag != 1)    local_lock_flag = 1;
//            }
//        }
//        else
//        {
//            local_lock_startflag = 0;
//            if(local_lock_flag !=1) local_cmd = LOCAL_CMD_STOP;//点动
//        }

//    }        
//}