////////////////////////////////////////////////////////////////////////////////
#ifndef __MAIN_H
#define __MAIN_H
////////////////////////////////////////////////////////////////////////////////
enum{
	REFRESH_ADC,
	REFRESH_TEMP,
	REFRESH_FLASH,
	REFRESH_I2C,
	REFRESH_CAN,
	REFRESH_RTC,
	REFRESH_UART1,
	REFRESH_UART2,
	REFRESH_IR,
	REFRESH_LED
};

typedef struct {
	u16 adc;
	u16 temp;
	u16 flash;
	u16 i2c;
	u16 can;
	u16 rtc;
	u16 uart1;
	u16 uart2;
	u16 ir;
	u16 led;
	u16 lcd;
}Peripheraldef;

////////////////////////////////////////////////////////////////////////////////
#ifdef _MAIN_C_
#define GLOBAL

#else
#define GLOBAL extern
#endif

GLOBAL Peripheraldef error;
GLOBAL Peripheraldef rf;
GLOBAL Peripheraldef rfCnt;
GLOBAL Peripheraldef rfOverCnt;

GLOBAL u8 testStart;
GLOBAL u16 drawBlockCnt;


#undef GLOBAL
////////////////////////////////////////////////////////////////////////////////

void initGPIO(void);



/******************************************************************************/
#endif
/******************************************************************************/



