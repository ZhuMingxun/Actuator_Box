#ifndef _LOCAL_H
#define _LOCAL_H

#define LOCAL_ON            0
#define LOCAL_STOP_ON       0

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

#define KEY_LOCAL 	1
#define KEY_STOP 	2
#define KEY_OPEN 	3
#define KEY_CLOSE 	4
#define KEY_REMOTE 	5
#define SCAN_DELAY_MS 10
#define SCAN_RMTDELAY_MS 100

#define LOCAL_STAT_STOP 			0
#define LOCAL_STAT_OPEN_INCHING 	1
#define LOCAL_STAT_OPEN_KEEP 		2
#define LOCAL_STAT_CLOSE_INCHING 	3
#define LOCAL_STAT_CLOSE_KEEP 		4

typedef enum
{
    LOCAL_CMD_STOP =0,     
    LOCAL_CMD_OPEN  ,    
    LOCAL_CMD_CLOSE        
}LocalCmd_Type;
extern LocalCmd_Type local_cmd;
extern  u8 local_stat;
void Local_Mode();
void Local_Control();
void DefZero_Mode();
void DefHundred_Mode();
void SetSen_Mode();
//void CalOut_Mode();
//void CalIn_Mode();
//void Exti23_Config();
void Exti23_Disable();
void CalHigh_Mode();
void CalLow_Mode();
u8 KeyScan(u8 key);
void SetInputLow_Mode();
void SetInputHigh_Mode();
#endif