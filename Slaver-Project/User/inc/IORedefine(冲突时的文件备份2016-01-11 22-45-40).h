/********************************************************************************/
/* This I/O Redefine header file is just suitable for the PCB of Version 2.0 !  
/* date:2015.2.4
/* author:ZhuMX

/********************************************************************************/
#ifndef _IORedefine_H_
#define _IORedefine_H_

/*****LCD°å****/

#define     HT1621B_CS          P00 //HT1621Bç‰‡é€‰ç«¯
#define     HT1621B_WR          P01 //HT1621Bå†™æ•°æ®ç«¯
#define     HT1621B_DATA        P02 //HT1621Bæ•°æ®ç«¯
#define     LED_RED             P03 
#define     LED_GREEN           P04 
#define     LED_YELLOW          P45 
#define     IR_RCV              P32 //çº¢å¤–æ¥æ”¶ç«¯ P32 INT0

///*Ö÷¿Ø°å°´¼ü*/
//#define     LED_LSG             P07 //Lose signal LED//#define     OPENING_0_DEF       P54 //0%±ê¶¨
//#define     OPENING_100_DEF     P40 //100%±ê¶¨
//#define     SET_SEN             P55 // ÉèÖÃ/ÁéÃô¶È

///*²¦ÂëÆ÷*/
//#define     PIN_LOSE_OPEN           P45 // ¶ªĞÅ¿ª
//#define     PIN_LOSE_CLOSE          P46 // ¶ªĞÅ¹Ø
//#define     POS_REV             P23 //Õı£¬·´×÷ÓÃ
//#define     ALARM               P24 //µç»ú10s±£»¤
//#define     ALARM_ON            0
//#define     ALARM_OFF           1
/*ÏÖ³¡ĞÅºÅ*/
//#define     LOCAL               P20 // ÏÖ³¡ĞÅºÅ
//#define     LOCAL_STOP          P21 // ÏÖ³¡Í£Ö¹
//#define     LOCAL_CLOSE         P37 // ÏÖ³¡¹ØĞÅºÅ
//#define     LOCAL_OPEN          P36 // ÏÖ³¡¿ªĞÅºÅ
///*Ô¶·½¿ØÖÆĞÅºÅ*/
//#define     PIN_DISTANT_STOP        P22   //Ô¶·½±£³Ö
//#define     PIN_DISTANT_OPEN        P26  //CCP1_3 Ô¶·½¿ª
//#define     PIN_DISTANT_CLOSE       P27  //CCP2_3 Ô¶·½¹Ø

///*ĞĞ³ÌÁ¦¾ØÏŞÎ»ĞÅºÅ*/                               
//#define     PIN_TORQUE_OPEN     P44 // Á¦¾ØÏŞÎ»       
//#define     PIN_LIM_OPN         P43 // ĞĞ³ÌÏŞÎ» 
//#define     PIN_LIM_CLE         P42 // ĞĞ³ÌÏŞÎ» 
//#define     PIN_TORQUE_CLOSE    P41 // Á¦¾ØÏŞÎ» 
//#define     PIN_FLAG_LIMOPN     P16
//#define     PIN_FLAG_LIMCLE     P17

//#define     PIN_FLAG_LIMCLE_ON  {if(PIN_FLAG_LIMCLE!=1)PIN_FLAG_LIMCLE = 1;}
//#define     PIN_FLAG_LIMOPN_ON  {if(PIN_FLAG_LIMOPN!=1)PIN_FLAG_LIMOPN = 1;}
//#define     PIN_FLAG_LIMCLE_OFF {if(PIN_FLAG_LIMCLE!=0)PIN_FLAG_LIMCLE = 0;}
//#define     PIN_FLAG_LIMOPN_OFF {if(PIN_FLAG_LIMOPN!=0)PIN_FLAG_LIMOPN = 0;}

///*ÏàĞò¼ì²â²¶»ñĞÅºÅ*/
//#define     U_CAP               P32 // INT0
//#define     V_CAP               P33 // INT1

///*µç»úÕı·´×ª¿ØÖÆĞÅºÅ*/
//#define     RELAY_A              P05  // ¼ÌµçÆ÷A
//#define     RELAY_B              P06  // ¼ÌµçÆ÷B

///*±¨´íĞÅºÅ*/
//#define     ERR                 P47
//#define     ERR_OUT             {if(ERR != 0) ERR = 0;}
//#define     ERR_OFF             {if(ERR != 1) ERR = 1;}

//#define     P_PWM              P35 //ÎªPWM

                               //P10,Input Current  Vi   ADC0    
                               //P11,Actual Opening Va   ADC1
                               //P12,Vref_2.5V      Vref ADC2
                               //P30£¬P31ÎªTXD,RXD


#endif

///**************************************************************************/
///* This I/O Redefine header file is just suitable for the PCB of Version 1.0 !  
///* date:
///* author:ZhuMX

///**************************************************************************/

//#ifndef _IORedefine_H_
//#define _IORedefine_H_


//#define     LCD_GREEN      P00 //LCDÂÌµÆ
//#define     LCD_RED        P01 //LCDºìµÆ
//#define     HT1621B_DATA   P02 //HT1621BÊı¾İ¶Ë
//#define     HT1621B_WR     P03 //HT1621BĞ´Êı¾İ¶Ë
//#define     HT1621B_CS     P04 //HT1621BÆ¬Ñ¡¶Ë
//#define     OPEN           P05  // ¼ÌµçÆ÷¿ª
//#define     CLOSE          P06  // ¼ÌµçÆ÷¹Ø
//#define     LSG_LED        P07  //¶ªĞÅ

//                               //P10,P11,P12ÎªADC²ÉÑùÊäÈë
//#define     LOSE_O         P13 // ¶ªĞÅ¿ª
//#define     LOSE_C         P14 // ¶ªĞÅ¹Ø
//#define     ZZY            P15 //Õı×÷ÓÃ
//#define     SET0           P45 //0%±ê¶¨
//#define     SET100         P24 //100%±ê¶¨

//#define     LMD            P47 // ÁéÃô¶È
//#define     ALARM          P23
//#define     LOC            P20 // ÏÖ³¡ĞÅºÅ
//#define     STOP           P21 // ÏÖ³¡±£³ÖĞÅºÅ
//#define     CLE            P37 // ÏÖ³¡¹ØĞÅºÅ
//#define     OPN            P36 // ÏÖ³¡¿ªĞÅºÅ
//#define     ERR            P22

//                                //P30£¬P31ÎªTXD,RXD
//                                //P35 ÎªPWM
//#define     shangB         P44 // Á¦¾ØÏŞÎ» ¿ªINT3
//#define     shangA         P43 // ĞĞ³ÌÏŞÎ» ¿ªINT2
//#define     xiaA           P42 // ĞĞ³ÌÏŞÎ» ¹ØINT1
//#define     xiaB           P41 // Á¦¾ØÏŞÎ» ¹ØINT0

//#endif



