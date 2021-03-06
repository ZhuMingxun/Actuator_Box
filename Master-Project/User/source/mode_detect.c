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
#include "mode_detect.h"
#include "local.h"
#include "com.h"
#include "actuator_config.h"
/********************丢信检测*****************/
void DistantAnalog_Detect()
{
    float I_in;
    I_in = pData_Acquire->current_input;
    
    #ifdef KAIGUAN_MODE
        mode = MODE_DISTANT_DIGITAL;
    #else
    if(I_in < 3.0f )
    {
        mode = MODE_DISTANT_DIGITAL;
    }
    else
    {
        mode = MODE_DISTANT_ANALOG;  
    }
    #endif
 
}

/**************系统模式检测**************/
void SystemMode_Detect()
{
    if(!DISTANT)
    {
        delay_ms(100);
        if(!DISTANT)
        {
            mode = MODE_LOCAL;
        }
    }
    else
    {
        
        mode = MODE_DISTANT_ANALOG;
        DistantAnalog_Detect();
    }    
}


