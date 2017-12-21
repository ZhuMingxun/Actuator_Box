#ifndef __MODE_DEF_H
#define __MODE_DEF_H
#include "timer.h"
#include "uart.h"
#include "ir.h"

//#define SEN0_05MA   50
//#define SEN0_1MA    100
//#define SEN0_15MA   150
//#define SEN0_2MA    200
//#define SEN0_25MA   250
//#define SEN0_3MA    300
//#define SEN0_35MA   350
//#define SEN0_4MA    400
//#define SEN0_45MA   450
//#define SEN0_5MA    500

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

typedef enum
{
    MODE_DISTANT_ANALOG = 0x00,
    MODE_LOCAL          = 0x01,              
    MODE_DISTANT_DIGITAL= 0x02,  
    MODE_LACK_PHASE     = 0x03,     
    MODE_DEF_ZERO       = 0x04,       
    MODE_DEF_HUNDRED    = 0x05,    
    MODE_OUT_CAL        = 0x06,       
    MODE_IN_CAL         = 0x07,        
    MODE_SET_SEN        = 0x08,
    MODE_CAL_LOW,
	MODE_CAL_HIGH,
	MODE_SETINPUT_LOW,
	MODE_SETINPUT_HIGH
}Mode_Type;    

typedef enum
{
    MOTOR_STATUS_OPEN  = 0,
    MOTOR_STATUS_CLOSE = 1, 
    MOTOR_STATUS_STOP  = 2
}MotorStatus_Type;


typedef struct
{
    float current_input;//输入电流
    float voltage_valve;//阀位电压
    float opening;//开度
    float current_output;//输出电流
    u16 sen;//灵敏度
    u16 uint_encoder_value;
    u8 motor_cr; //电机保护标志
    u8 valve_position;//阀门位置
    u8 encoder_flag;//编码器标志
    Mode_Type mode;//工作模式
    MotorStatus_Type motor_status;//电机状态
    
}MasterData_TypeDef;

extern MasterData_TypeDef* pMasterData;
void Timer1_Config();
void MasterData_Init();
void DistantAnalog_Mode();
void DistantDigital_Mode();
void Local_Mode();
void LackPhase_Mode();
void DefZero_Mode();
void DefHundred_Mode();
void SetSen_Mode();
void CalOut_Mode();
void CalIn_Mode();
void CalHigh_Mode();
void CalLow_Mode();
void Set_Input_Low();
void Set_Input_High();

#endif