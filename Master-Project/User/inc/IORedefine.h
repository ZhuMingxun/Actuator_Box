/********************************************************************************/
/* This I/O Redefine header file is just suitable for the PCB of Version 2.0 !  
/* date:2015.2.4
/* author:ZhuMX

/********************************************************************************/
#ifndef _IORedefine_H_
#define _IORedefine_H_

/*****LCD∞Â****/

//#define     HT1621B_CS          P00 //HT1621BÁâáÈÄâÁ´Ø
//#define     HT1621B_WR          P01 //HT1621BÂÜôÊï∞ÊçÆÁ´Ø
//#define     HT1621B_DATA        P02 //HT1621BÊï∞ÊçÆÁ´Ø
#define     PIN_FLAG_LIMOPN     P04
#define     PIN_FLAG_LIMCLE     P03

#define     PIN_FLAG_LIMCLE_ON  {PIN_FLAG_LIMCLE = 1;PIN_FLAG_LIMOPN = !PIN_FLAG_LIMCLE;}
#define     PIN_FLAG_LIMOPN_ON  {PIN_FLAG_LIMOPN = 1;PIN_FLAG_LIMCLE = !PIN_FLAG_LIMOPN;}
#define     PIN_FLAG_LIMCLE_OFF {PIN_FLAG_LIMCLE = 0;}
#define     PIN_FLAG_LIMOPN_OFF {PIN_FLAG_LIMOPN = 0;}

///*≤¶¬Î∆˜*/
#define     PIN_LOSE_OPEN           P45 // ∂™–≈ø™
#define     PIN_LOSE_CLOSE          P46 // ∂™–≈πÿ
#define     POS_REV             P23 //’˝£¨∑¥◊˜”√
#define     PIN_ALARM               P24 //µÁª˙10s±£ª§
#define     ALARM_ON            0
#define     ALARM_OFF           1
/*œ÷≥°–≈∫≈*/
#define     LOCAL               P20 // œ÷≥°–≈∫≈
#define     LOCAL_STOP          P21 // œ÷≥°Õ£÷π
#define     LOCAL_CLOSE         P37 // œ÷≥°πÿ–≈∫≈
#define     LOCAL_OPEN          P36 // œ÷≥°ø™–≈∫≈
///*‘∂∑Ωøÿ÷∆–≈∫≈*/
#define     PIN_DISTANT_STOP        P22   //‘∂∑Ω±£≥÷
#define     PIN_DISTANT_OPEN        P26  //CCP1_3 ‘∂∑Ωø™
#define     PIN_DISTANT_CLOSE       P27  //CCP2_3 ‘∂∑Ωπÿ

///*––≥Ã¡¶æÿœﬁŒª–≈∫≈*/                               
#define     PIN_TORQUE_OPEN     P44 // ¡¶æÿœﬁŒª       
#define     PIN_LIM_OPN         P43 // ––≥ÃœﬁŒª 
#define     PIN_LIM_CLE         P42 // ––≥ÃœﬁŒª 
#define     PIN_TORQUE_CLOSE    P41 // ¡¶æÿœﬁŒª 
//#define     PIN_FLAG_LIMOPN     P16
//#define     PIN_FLAG_LIMCLE     P17

//#define     PIN_FLAG_LIMCLE_ON  {if(PIN_FLAG_LIMCLE!=1)PIN_FLAG_LIMCLE = 1;}
//#define     PIN_FLAG_LIMOPN_ON  {if(PIN_FLAG_LIMOPN!=1)PIN_FLAG_LIMOPN = 1;}
//#define     PIN_FLAG_LIMCLE_OFF {if(PIN_FLAG_LIMCLE!=0)PIN_FLAG_LIMCLE = 0;}
//#define     PIN_FLAG_LIMOPN_OFF {if(PIN_FLAG_LIMOPN!=0)PIN_FLAG_LIMOPN = 0;}

///*œ‡–ÚºÏ≤‚≤∂ªÒ–≈∫≈*/
#define     U_CAP               P32 // INT0
#define     V_CAP               P33 // INT1

///*µÁª˙’˝∑¥◊™øÿ÷∆–≈∫≈*/
#define     RELAY_A              P05  // ºÃµÁ∆˜A
#define     RELAY_B              P06  // ºÃµÁ∆˜B

///*±®¥Ì–≈∫≈*/
#define     ERR                 P47
#define     ERR_OUT             {if(ERR != 0) ERR = 0;}
#define     ERR_OFF             {if(ERR != 1) ERR = 1;}

#define     P_PWM              P35 //Œ™PWM

                               //P10,Input Current  Vi   ADC0    
                               //P11,Actual Opening Va   ADC1
                               //P12,Vref_2.5V      Vref ADC2
                               //P30£¨P31Œ™TXD,RXD


#endif
