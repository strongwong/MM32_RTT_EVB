////////////////////////////////////////////////////////////////////////////////
#define _LED_KEY_C_
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "EVBoard.h"


////////////////////////////////////////////////////////////////////////////////
void initGPIO_LED()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	// LD2
	LED_2_OFF();
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// LD1
	LED_1_OFF();
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;//GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

////////////////////////////////////////////////////////////////////////////////
void initGPIO_KEY()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// Key
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;			// pb2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;			// pb3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;			// pb4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//////////////////////////////////////////////////////////////////////////////////
void LED_2_ON()			{ GPIO_ResetBits(GPIOA, GPIO_Pin_8);}
void LED_2_OFF()		{ GPIO_SetBits  (GPIOA, GPIO_Pin_8);}
void LED_1_ON()			{ GPIO_ResetBits(GPIOC, GPIO_Pin_13);}
void LED_1_OFF()		{ GPIO_SetBits  (GPIOC, GPIO_Pin_13);}


////////////////////////////////////////////////////////////////////////////////
void ProcessLED()
{
	switch (ledCnt++) {
		case 0:		LED_2_ON(); 	break;
		case 1:		LED_2_OFF(); 	break;; 
		case 2:		LED_1_ON(); 	break;
		default:	LED_1_OFF();	
					ledCnt = 0;		break;
	}
	rf.led = TRUE;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void modifyKeyStatus_2()
{
	char str[8];
	if (menuCnt == 3) {
		(Key2CntF) ? strcpy(str, "Last") : strcpy(str, "Next");
		showButton(255, 15 + 2 * offsetH, 1, str);
	}
}

////////////////////////////////////////////////////////////////////////////////
void modifyKeyStatus_3()
{
	char str[8];
	if (menuCnt == 3) {
		(Key3CntF) ? strcpy(str, "Down") : strcpy(str, "Up");
		showButton(255, 15 + 3 * offsetH, 1, str);
	}
}

////////////////////////////////////////////////////////////////////////////////
void scanKey()
{
	if (Key1 && !fKeyP1) {
		fKeyP1 = TRUE;
		fKey1 = TRUE;
		fKeyCnt = 0;
	}
	else if (!Key1) {
		fKeyP1 = FALSE;
	}		
	else {
		fKeyCnt = 0;
	}
//-----------------	
	if (!Key2 && !fKeyP2) {
		fKeyP2 = TRUE;
		fKey2 = TRUE;
		fKeyCnt = 0;
	}
	else if (Key2) {
		fKeyP2 = FALSE;
		Key2Cnt = 0;
	}		
	else {
		fKeyCnt = 0;
		if ((Key2Cnt < 2001) && (Key2Cnt++ >= 2000)){
			Key2CntF = !Key2CntF;
			modifyKeyStatus_2();
		}
	}
//-----------------	
	if (!Key3 && !fKeyP3) {
		fKeyP3 = TRUE;
		fKey3 = TRUE;
		fKeyCnt = 0;
	}
	else if (Key3) {
		fKeyP3 = FALSE;
		Key3Cnt = 0;
	}
	else {
		fKeyCnt = 0;
		if ((Key3Cnt < 1501) && (Key3Cnt++ >= 1500)){
			Key3CntF = !Key3CntF;
			modifyKeyStatus_3();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void clearMenu()
{
	menuTIME(timeSel, 0, 0);
	dispScreen();
}


////////////////////////////////////////////////////////////////////////////////
void showButton(u16 x, u16 y, u8 frame, char* str)
{
	drawButton(x, y, 60, 20, frame, 0, &str[0]);
}

////////////////////////////////////////////////////////////////////////////////
void dispButton()
{
	char str[8];
	switch (menuCnt) {
	case 0:
		strcpy(str, "Reset");
		showButton(255, 15 + 0 * offsetH, 1, str);
		
		strcpy(str, "Menu");
		showButton(255, 15 + 1 * offsetH, 1, str);
		
		if (uartSel == 0x01)
			strcpy(str, "UART");
		else
			strcpy(str, "----");
		showButton(255, 15 + 2 * offsetH, 1, str);
		
		strcpy(str, "IR_TX");
		showButton(255, 15 + 3 * offsetH, 1, str);
		break;
	case 1:
	case 2:
		strcpy(str, "Reset");
		showButton(255, 15 + 0 * offsetH, 1, str);
		
		strcpy(str, "Menu");
		showButton(255, 15 + 1 * offsetH, 1, str);
		
		strcpy(str, "Next");
		showButton(255, 15 + 2 * offsetH, 1, str);
		
		strcpy(str, "----");
		showButton(255, 15 + 3 * offsetH, 0, str);
		break;
	default:
		strcpy(str, "Reset");
		showButton(255, 15 + 0 * offsetH, 1, str);
		
		strcpy(str, "Return");
		showButton(255, 15 + 1 * offsetH, 1, str);
		
		(Key2CntF) ? strcpy(str, "Last") : strcpy(str, "Next");
		showButton(255, 15 + 2 * offsetH, 1, str);
		
		if (timeSel == 6) {
			strcpy(str, "Save");
			showButton(255, 15 + 3 * offsetH, 1, str);
		}
		else {
			(Key3CntF) ? strcpy(str, "Down") : strcpy(str, "Up");
			showButton(255, 15 + 3 * offsetH, 1, str);
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
void dispMenu(u8 sta)
{
	switch (menuCnt) {
	case 0:										break;
	case 1:		menuCAN(canSel, 1, sta);		break;
	case 2:		menuUART(uartSel, 1, sta);		break;
	default:	menuTIME(timeSel, 1, sta);		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
u8 eventKey(u8 key, u8* flag)
{
	if (!*flag) return 0;

	*flag = FALSE;

	switch (key) {
	case 0:											// menuOverTime
		if (timeSel == 6) timeSel = 0; 
		menuCnt = 0;
		return 2;
		
	case 1:											// Menu
		if (++menuCnt > 3)  menuCnt = 0;
		
		switch(menuCnt){
		case 0:
			return 2;
		case 3:
			Key2CntF = FALSE;
			Key3CntF = FALSE;
			RTC_GetTime(&gtp);
			if (timeSel == 6) timeSel = 0;
		default:			
			return 1;
		}
		
	case 2:			// Next & Last
		switch (menuCnt) {
		case 0:			
			if (uartSel == 0x01)
				UART_SendData(UART1,  0x01);
			break;
		case 1:
			if (++canSel > 2) canSel = 0;
			BKP_WriteBackupRegister(BKP_DR2, BKP_ReadBackupRegister(BKP_DR2) & ~0x03 | canSel);
			initCAN();
			break;
		case 2:
			if (++uartSel > 2) uartSel = 0;
			BKP_WriteBackupRegister(BKP_DR2, BKP_ReadBackupRegister(BKP_DR2) & ~0x0C | uartSel << 2);
			break;
		default:
			if (Key2CntF) 	{if (--timeSel == -1) 	timeSel = 6;}
			else 			{if (++timeSel > 6) 	timeSel = 0;}
			break;
		}
		return 3;
		
	default:										// Up & Down & Save & IR_TX
		switch (menuCnt) {
		case 3:
			modifyTime(timeSel);
			if (timeSel == 6) {
				menuCnt = 0;
				return 2;
			}
		case 0:
			if (!irTxF) {
				irTxF = TRUE;
				*(u32*)(txbuf + 8) = (u32)RTC_GetCounter();
				initIR_Tx();
			}
		default:
			break;
		}
		return 3;
	}
}

////////////////////////////////////////////////////////////////////////////////
void dispMenuButton(u8 idx)
{
	switch (idx) {
	case 0:
		break;
	case 1:
		dispMenu(0);
		dispButton();
		break;
	case 2:
		clearMenu();
	default:
		dispMenu(1);
		dispButton();
		break;
	}
}

