#ifndef _DATA_SAVE_H_
#define _DATA_SAVE_H_



#define     ADDR_OPENING_0      0
#define     ADDR_OPENING_100    512
#define     ADDR_SEN            (ADDR_OPENING_0+2)            
#define     ADDR_R_IN           (ADDR_SEN+2)
#define     ADDR_R_OUT          (ADDR_OPENING_100+2)


void IapWrite_Opening0_adc(u16 adc_data);
void IapWrite_Opening100_adc(u16 adc_data);
void IapWrite_Sen(u16 sen_data);
void IapWrite_R_in(float res);
void IapWrite_R_out(float res);

void IapReadData();

u16 IapRead_Sen();
u16 IapRead_Opening0_adc();
u16 IapRead_Opening100_adc();
float IapRead_R_IN();
float IapRead_R_OUT();



#endif