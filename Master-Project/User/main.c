
#include "main.h"
#include "com.h"
#include "mlx90316.h"
#include "actuator_config.h"
void main()
{  
	delay_ms(500);
    //UART1_SWToDebug();
    //printf("\r\n Start! ");
    Motor_PosRev();//printf("\r\n Motor_PosRev() ");	
    Motor_Config();//printf("\r\n Motor_Config()! ");
    Timer1_Config();//printf("\r\n Timer1_Config()! ");
    ADC_Config();//printf("\r\n ADC_Config()! ");
    PWMIO_Config();//printf("\r\n PWMIO_Config()! ");
    Timer0_Config();//printf("\r\n Timer0_Config()! ");
    MLX90316_SPI_Config();//printf("\r\n MLX90316_SPI_Config()! ");
    
    #ifdef PHASE_SEQ
    PhaseSeq_Detect_Config();//printf("\r\n PhaseSeq_Detect_Config()! ");
    #endif

    SystemParam_Init();//printf("\r\n SystemParam_Init()! ");
	
    SystemMode_Detect();//printf("\r\n SystemMode_Detect()! ");
    Detect_ValveInput();//printf("\r\n Detect_ValveInput()! ");
    DataSampInit();//printf("\r\n DataSampInit()! ");

    Communication_Config();
    
    while(1)
    {
        switch(mode)
        {
            case MODE_DISTANT_ANALOG:   {DistantAnalog_Mode();  break;}
            case MODE_LOCAL:            {Local_Mode();          break;}
            case MODE_DISTANT_DIGITAL:  {DistantDigital_Mode(); break;}
            case MODE_DEF_ZERO:         {DefZero_Mode();        break;}
            case MODE_DEF_HUNDRED:      {DefHundred_Mode();     break;}
            case MODE_SET_SEN:          {SetSen_Mode();         break;}
            case MODE_CAL_LOW:			{CalLow_Mode();break;}
			case MODE_CAL_HIGH:			{CalHigh_Mode();break;}
			case MODE_SETINPUT_LOW:  	{SetInputLow_Mode();break;}
			case MODE_SETINPUT_HIGH: 	{SetInputHigh_Mode();break;}	
            #ifdef PHASE_SEQ
            case MODE_LACK_PHASE:       {LackPhase_Mode();break;}
            #endif
            
            default:break;
   
        }
                

    }

}
       
        
