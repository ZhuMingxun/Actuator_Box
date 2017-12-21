/********************************************************************************/
/* This I/O Redefine header file is just suitable for the PCB of Version 2.0 !  
/* date:2015.2.4
/* author:ZhuMX

/********************************************************************************/
#ifndef _IORedefine_H_
#define _IORedefine_H_
//#define __OLDLCD
//#define __NEWLCD_44P
#define __NEWLCD_20P
/*****LCD��****/
#ifdef __OLDLCD

#define     HT1621B_CS          P00 //HT1621B片选端
#define     HT1621B_WR          P01 //HT1621B写数据端
#define     HT1621B_DATA        P02 //HT1621B数据端
#define     LED_RED             P03 
#define     LED_GREEN           P04 
#define     LED_YELLOW          P45 
#define     IR_RCV              P32 //红外接收端 P32 INT0

//===================================================================
#elif defined __NEWLCD_44P

#define     HT1621B_WR          P00 //HT1621B片选端
#define     HT1621B_DATA        P01 //HT1621B写数据端
#define     LED_RED             P02 //HT1621B数据端
#define     LED_YELLOW          P03 
#define     LED_GREEN           P04 
#define     HT1621B_CS P46 
#define     IR_RCV              P32 //红外接收端 P32 INT0

//===================================================================
#else

#define     LED_RED             P33 //HT1621B数据端
#define     HT1621B_CS          P35 
#define     HT1621B_WR          P36 //HT1621B片选端
#define     HT1621B_DATA        P37 //HT1621B写数据端

#define     LED_YELLOW          P13 
#define     LED_GREEN           P12 

#define     IR_RCV              P32 //红外接收端 P32 INT0


#endif

#endif




