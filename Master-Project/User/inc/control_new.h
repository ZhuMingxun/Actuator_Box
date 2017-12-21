#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "motor.h"


#define LIM_ON  1
#define TORQUE_ON 1

typedef enum
{
    MODE_DISTANT_ANALOG = 0,
    MODE_LOCAL             ,              
    MODE_DISTANT_DIGITAL   ,  
    MODE_LACK_PHASE        ,     
    MODE_DEF_ZERO          ,       
    MODE_DEF_HUNDRED       ,    
    MODE_OUT_CAL           ,       
    MODE_IN_CAL            ,        
    MODE_SET_SEN           ,
	MODE_CAL_LOW			,
	MODE_CAL_HIGH			,
	MODE_SETINPUT_LOW		,
	MODE_SETINPUT_HIGH
	
}Mode_Type;    extern volatile Mode_Type mode; 

/* adaptive position control struct */
/*
    pos_target = pos_input - offset_open_filter(offset_close_filter)
    offset_open = pos_real - pos_target
    offset_close = pos_real - pos_target
    offset_open_filter = offset_open_filter+K_OFFSET*(offset_open-offset_open_filter)
    offset_open_filter = offset_close_filter+K_OFFSET*(offset_close-offset_close_filter)
*/
#define K_OFFSET 0.8
typedef struct{
    
    float pos_input;
    float pos_target;
    float pos_real;
    float offset_open;
    float offset_close;
    float offset_open_filter;
    float offset_close_filter; 
    
}PositionControl_TypeDef;

#define DISTANT         (LOCAL != LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
#define T1_MS           20  //20ms
#define T1_CNT          (65536UL - ((T1_MS*MAIN_Fosc)/12/1000))

void Timer1_Config();

extern u8  timer1_20ms_flag;
extern u8  timer1_60ms_flag;
extern u8  timer1_500ms_flag;
extern u8  timer1_200ms_flag;
extern u16 timer1_100ms_flag;
extern u8  timer1_400ms_flag;
extern u8  timer1_1s_flag;
#endif