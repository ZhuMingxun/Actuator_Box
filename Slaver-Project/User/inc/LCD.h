/************************************************************************************
文件名 ：LCD.h
作者   ：朱明勋
说明   ：LCD显示函数声明，用来显示屏幕上的数据

*************************************************************************************/



#ifndef _LCD_H
#define _LCD_H

#include "IORedefine.h"
#define     LED_RED_ON          {if(LED_RED != 1) LED_RED = 1;}          
#define     LED_RED_OFF         {if(LED_RED != 0) LED_RED = 0;}         
#define     LED_GREEN_ON        {if(LED_GREEN != 1) LED_GREEN = 1;}            
#define     LED_GREEN_OFF       {if(LED_GREEN != 0) LED_GREEN = 0;}    
#define     LED_YELLOW_ON       {if(LED_YELLOW != 0) LED_YELLOW = 0;}    
#define     LED_YELLOW_OFF      {if(LED_YELLOW != 1) LED_YELLOW = 1;}

void LCD_Init();
void RealTime_Opening_DIS(float float_data);
void LCD_FullDisplay(); 
void LCD_ClearDisplay(); 
void Def0_DIS();
void Def100_DIS();
void InputCurrent_DIS(float f_data);
void Voltage_DIS(float f_data);
void Local_DIS();
void LCD_TorqueDIS_500ms();
void LED_MotorDIS_500ms();
void LCD_DIS();
void OutputCurrent_DIS(float f_data);
void Sen_DIS(u16 sen_value);
void LCD_LED_DIS();
void LCD_TEST();

void LCD_SetDIS();

void LO_Alarm_DIS();
void LC_Alarm_DIS();
void EP_Alarm_DIS();
void Clear_Alarm_DIS();

void CalLow_DIS();
void CalHigh_DIS();

void Set_InputLow_DIS();
void Set_InputHigh_DIS();

//extern u8  ep_discnt;
//extern u8  lo_discnt;
//extern u8  lc_discnt;
//extern u8  lcd_redcnt;
//extern u8  lcd_greencnt;
//extern u8  ep_dis;
//extern u8  lo_dis;
//extern u8  lc_dis;
//extern u8  current_out_flag;

#endif