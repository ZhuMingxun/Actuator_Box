#include "mode_def.h"
#include "LCD.h"
#include "uart.h"


void main()
{
    LCD_Init();
    Timer1_Config();
    Communication_Config();
    //UART2_DebugConfig();
    MasterData_Init();
   
    while(1)
    {
        if(MasterIsReadyToRead())
        {    
            MasterData_Update();
            switch(pMasterData->mode)
            {
                case MODE_DISTANT_ANALOG:   {DistantAnalog_Mode();  break;}
                case MODE_DISTANT_DIGITAL:  {DistantDigital_Mode(); break;}
                case MODE_LOCAL:            {Local_Mode();          break;}
                case MODE_DEF_ZERO:         {DefZero_Mode();        break;}
                case MODE_DEF_HUNDRED:      {DefHundred_Mode();     break;}
                case MODE_SET_SEN:          {SetSen_Mode();         break;}
				case MODE_CAL_LOW:			{CalLow_Mode();			break;}
				case MODE_CAL_HIGH:			{CalHigh_Mode();		break;}
				case MODE_SETINPUT_LOW:		{Set_Input_Low();		break;}
				case MODE_SETINPUT_HIGH:	{Set_Input_High();		break;}
                case MODE_LACK_PHASE:       {LackPhase_Mode();      break;}
                default:break;
            }
        }
  
    }
   
}
