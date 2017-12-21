#ifndef _COM_H
#define _COM_H

#include "config.h"
#include "control_new.h"
#include "motor.h"




/* 发送帧数据类型 */
typedef struct
{
    u8 uc_current_input;//输入电流
    u8 uc_opening;//阀位开度
    u8 motor_cr; //电机保护标志
    u8 valve_position;//阀门位置
    u8 encoder_flag;//编码器标志
    u16 uint_voltage_valve;//阀门位置电压
    u16 uint_encoder_value;
    u16 sen;//灵敏度
    Mode_Type workmode;//工作模式
    MotorStatus_Type motor_stat;//电机状态
    
}MasterTxData_TypeDef;
extern MasterTxData_TypeDef* pMasterTxData;
extern u8 flag_tx_busy;
void Communication_Config();
void TransmitFrameToSlaver();
u8 SlaverIsReadyToRead();
void SlaverData_Update();
void UART1_SWToDebug();






#endif




















