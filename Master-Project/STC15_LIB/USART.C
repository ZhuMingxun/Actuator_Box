
#include "USART.h"


COMx_Define	COM1,COM2;
u8	xdata TX1_Buffer[COM_TX1_Lenth];	//∑¢ÀÕª∫≥Â
u8 	xdata RX1_Buffer[COM_RX1_Lenth];	//Ω” ’ª∫≥Â
u8	xdata TX2_Buffer[COM_TX2_Lenth];	//∑¢ÀÕª∫≥Â
u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//Ω” ’ª∫≥Â

u8 USART_Configuration(u8 UARTx, COMx_InitDefine *COMx)
{
	u8	i;
	u32	j;
	
	if(UARTx == USART1)
	{
		COM1.id = 1;
		COM1.TX_read    = 0;
		COM1.TX_write   = 0;
		COM1.B_TX_busy  = 0;
		COM1.RX_Cnt     = 0;
		COM1.RX_TimeOut = 0;
		COM1.B_RX_OK    = 0;
		for(i=0; i<COM_TX1_Lenth; i++)	TX1_Buffer[i] = 0;
		for(i=0; i<COM_RX1_Lenth; i++)	RX1_Buffer[i] = 0;

		if(COMx->UART_Mode > UART_9bit_BRTx)	return 2;	//ƒ£ Ω¥ÌŒÛ
		if(COMx->UART_Polity == PolityHigh)		PS = 1;	//∏ﬂ”≈œ»º∂÷–∂œ
		else									PS = 0;	//µÕ”≈œ»º∂÷–∂œ
		SCON = (SCON & 0x3f) | COMx->UART_Mode;
		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//ø…±‰≤®Ãÿ¬ 
		{
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//∞¥1Tº∆À„
			if(j >= 65536UL)	return 2;	//¥ÌŒÛ
			j = 65536UL - j;
			if(COMx->UART_BRT_Use == BRT_Timer1)
			{
				TR1 = 0;
				AUXR &= ~0x01;		//S1 BRT Use Timer1;
				TMOD &= ~(1<<6);	//Timer1 set As Timer
				TMOD &= ~0x30;		//Timer1_16bitAutoReload;
				AUXR |=  (1<<6);	//Timer1 set as 1T mode
				TH1 = (u8)(j>>8);
				TL1 = (u8)j;
				ET1 = 0;	//Ω˚÷π÷–∂œ
				TMOD &= ~0x40;	//∂® ±
				INT_CLKO &= ~0x02;	//≤ª ‰≥ˆ ±÷”
				TR1  = 1;
			}
			else if(COMx->UART_BRT_Use == BRT_Timer2)
			{
				AUXR &= ~(1<<4);	//Timer stop
				AUXR |= 0x01;		//S1 BRT Use Timer2;
				AUXR &= ~(1<<3);	//Timer2 set As Timer
				AUXR |=  (1<<2);	//Timer2 set as 1T mode
				TH2 = (u8)(j>>8);
				TL2 = (u8)j;
				IE2  &= ~(1<<2);	//Ω˚÷π÷–∂œ
				AUXR &= ~(1<<3);	//∂® ±
				AUXR |=  (1<<4);	//Timer run enable
			}
			else return 2;	//¥ÌŒÛ
		}
		else if(COMx->UART_Mode == UART_ShiftRight)
		{
			if(COMx->BaudRateDouble == ENABLE)	AUXR |=  (1<<5);	//πÃ∂®≤®Ãÿ¬ SysClk/2
			else								AUXR &= ~(1<<5);	//πÃ∂®≤®Ãÿ¬ SysClk/12
		}
		else if(COMx->UART_Mode == UART_9bit)	//πÃ∂®≤®Ãÿ¬ SysClk*2^SMOD/64
		{
			if(COMx->BaudRateDouble == ENABLE)	PCON |=  (1<<7);	//πÃ∂®≤®Ãÿ¬ SysClk/32
			else								PCON &= ~(1<<7);	//πÃ∂®≤®Ãÿ¬ SysClk/64
		}
		if(COMx->UART_Interrupt == ENABLE)	ES = 1;	//‘ –Ì÷–∂œ
		else								ES = 0;	//Ω˚÷π÷–∂œ
		if(COMx->UART_RxEnable == ENABLE)	REN = 1;	//‘ –ÌΩ” ’
		else								REN = 0;	//Ω˚÷πΩ” ’
		P_SW1 = (P_SW1 & 0x3f) | (COMx->UART_P_SW & 0xc0);	//«–ªªIO
		if(COMx->UART_RXD_TXD_Short == ENABLE)	PCON2 |=  (1<<4);	//ƒ⁄≤ø∂Ã¬∑RXD”ÎTXD, ◊ˆ÷–ºÃ, ENABLE,DISABLE
		else									PCON2 &= ~(1<<4);
		return	0;
	}

	if(UARTx == USART2)
	{
		COM2.id = 2;
		COM2.TX_read    = 0;
		COM2.TX_write   = 0;
		COM2.B_TX_busy  = 0;
		COM2.RX_Cnt     = 0;
		COM2.RX_TimeOut = 0;
		COM2.B_RX_OK    = 0;
		for(i=0; i<COM_TX2_Lenth; i++)	TX2_Buffer[i] = 0;
		for(i=0; i<COM_RX2_Lenth; i++)	RX2_Buffer[i] = 0;

		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//ø…±‰≤®Ãÿ¬ 
		{
			if(COMx->UART_Polity == PolityHigh)		IP2 |=  1;	//∏ﬂ”≈œ»º∂÷–∂œ
			else									IP2 &= ~1;	//µÕ”≈œ»º∂÷–∂œ
			if(COMx->UART_Mode == UART_9bit_BRTx)	S2CON |=  (1<<7);	//9bit
			else									S2CON &= ~(1<<7);	//8bit
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//∞¥1Tº∆À„
			if(j >= 65536UL)	return 2;	//¥ÌŒÛ
			j = 65536UL - j;
			AUXR &= ~(1<<4);	//Timer stop
			AUXR &= ~(1<<3);	//Timer2 set As Timer
			AUXR |=  (1<<2);	//Timer2 set as 1T mode
			TH2 = (u8)(j>>8);
			TL2 = (u8)j;
			IE2  &= ~(1<<2);	//Ω˚÷π÷–∂œ
			AUXR |=  (1<<4);	//Timer run enable
		}
		else	return 2;	//ƒ£ Ω¥ÌŒÛ
		if(COMx->UART_Interrupt == ENABLE)	IE2   |=  1;		//‘ –Ì÷–∂œ
		else								IE2   &= ~1;		//Ω˚÷π÷–∂œ
		if(COMx->UART_RxEnable == ENABLE)	S2CON |=  (1<<4);	//‘ –ÌΩ” ’
		else								S2CON &= ~(1<<4);	//Ω˚÷πΩ” ’
		P_SW2 = (P_SW2 & ~1) | (COMx->UART_P_SW & 0x01);	//«–ªªIO
	}
	return 0;
}


/*************** ◊∞‘ÿ¥Æø⁄∑¢ÀÕª∫≥Â *******************************/

//void TX1_write2buff(u8 dat)	//–¥»Î∑¢ÀÕª∫≥Â£¨÷∏’Î+1
//{
//	TX1_Buffer[COM1.TX_write] = dat;	//◊∞∑¢ÀÕª∫≥Â
//	if(++COM1.TX_write >= COM_TX1_Lenth)	COM1.TX_write = 0;

//	if(COM1.B_TX_busy == 0)		//ø’œ–
//	{  
//		COM1.B_TX_busy = 1;		//±Í÷æ√¶
//		TI = 1;					//¥•∑¢∑¢ÀÕ÷–∂œ
//	}
//}

//void PrintString1(u8 *puts)
//{
//    for (; *puts != 0;	puts++)  TX1_write2buff(*puts); 	//”ˆµΩÕ£÷π∑˚0Ω· ¯
//}


//void TX2_write2buff(u8 dat)	//–¥…´◊¢Ãç€∫‘•√¨÷∏÷´+1
//{
//	TX2_Buffer[COM2.TX_write] = dat;	//◊∞◊¢Ãç€∫‘•
//	if(++COM2.TX_write >= COM_TX2_Lenth)	COM2.TX_write = 0;

//	if(COM2.B_TX_busy == 0)		//‡†ï–ê
//	{  
//		COM2.B_TX_busy = 1;		//“™÷æ√¶
//		SET_TI2();				//‘•◊¢◊¢Ãç◊ê◊è
//	}
//}

//void PrintString2(u8 *puts)
//{
//    for (; *puts != 0;	puts++)  TX2_write2buff(*puts); 	//”ˆµΩÕ£÷π∑˚0Ω· ¯
//}

/*
void COMx_write2buff(COMx_Define *COMx, u8 dat)	//–¥»Î∑¢ÀÕª∫≥Â£¨÷∏’Î+1
{
	if(COMx->id == 1)	TX1_write2buff(dat);
	if(COMx->id == 2)	TX2_write2buff(dat);
}

void PrintString(COMx_Define *COMx, u8 *puts)
{
    for (; *puts != 0;	puts++)  COMx_write2buff(COMx,*puts); 	//”ˆµΩÕ£÷π∑˚0Ω· ¯
}
*/


/********************* UART1÷–∂œ∫Ø ˝************************/
//void UART1_int (void) interrupt UART1_VECTOR
//{
//	if(RI)
//	{
//		RI = 0;
//		if(COM1.B_RX_OK == 0)
//		{
//			if(COM1.RX_Cnt >= COM_RX1_Lenth)	COM1.RX_Cnt = 0;
//			RX1_Buffer[COM1.RX_Cnt++] = SBUF;
//			COM1.RX_TimeOut = TimeOutSet1;
//		}
//	}

//	if(TI)
//	{
//		TI = 0;
//		if(COM1.TX_read != COM1.TX_write)
//		{
//		 	SBUF = TX1_Buffer[COM1.TX_read];
//			if(++COM1.TX_read >= COM_TX1_Lenth)		COM1.TX_read = 0;
//		}
//		else	COM1.B_TX_busy = 0;
//	}
//}

/********************* UART2÷–∂œ∫Ø ˝************************/
//void UART2_int (void) interrupt UART2_VECTOR
//{
//	if(RI2)
//	{
//		CLR_RI2();
//		if(COM2.B_RX_OK == 0)
//		{
//			if(COM2.RX_Cnt >= COM_RX2_Lenth)	COM2.RX_Cnt = 0;
//			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;
//			COM2.RX_TimeOut = TimeOutSet2;
//		}
//	}

//	if(TI2)
//	{
//		CLR_TI2();
//		if(COM2.TX_read != COM2.TX_write)
//		{
//		 	S2BUF = TX2_Buffer[COM2.TX_read];
//			if(++COM2.TX_read >= COM_TX2_Lenth)		COM2.TX_read = 0;
//		}
//		else	COM2.B_TX_busy = 0;
//	}

//}



