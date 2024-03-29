////////////////////////////////////////////////////////////////////////////////
#ifndef __LCD_DEMO_H_
#define __LCD_DEMO_H_
////////////////////////////////////////////////////////////////////////////////

#define  DMAX 	80



////////////////////////////////////////////////////////////////////////////////
#ifdef _LCD_DEMO_C_
#define GLOBAL

u16 dx = 5;
u16 dy = 10;
u16 dw = 242;
u16 dh = 135;

#else
#define GLOBAL extern
#endif

extern u16 dx;
extern u16 dy;
extern u16 dw;
extern u16 dh;


#undef GLOBAL
////////////////////////////////////////////////////////////////////////////////
void dispADC(u8 sta);
void dispTEMP(u8 sta);
void dispFLASH(u8 sta);
void dispCAN(u8 sta);
void dispUART1(u8 sta);
void dispUART2(u8 sta);
void dispLED(u8 sta);
void dispI2C(u8 sta);
void dispRTC(u8 sta);
void dispIR(u8 sta);

void menuCAN(u8 idx, u8 sta, u8 sel);
void menuUART(u8 idx, u8 sta, u8 sel);
void menuTIME(u8 idx, u8 sta, u8 sel);


void initLCD();
void initSPI_LCD();
void randRefresh(u16* flag);
void initLcdDemo();
void refreshLCD(u8 sele, u16* flag);
void dispScreen();

void clearLeftScreen();
////////////////////////////////////////////////////////////////////////////////

void BootShow();

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
