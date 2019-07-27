////////////////////////////////////////////////////////////////////////////////
#ifndef __EVBoard_H_
#define __EVBoard_H_
////////////////////////////////////////////////////////////////////////////////

#include "sys.h"
#include "HAL_device.h"
//#include "main.h"

#include "adc.h"
#include "can.h"
#include "led_key.h"
#include "i2c.h"
#include "lcd.h"
#include "spi.h"
#include "uart.h"
#include "timers.h"
#include "rtc.h"	  
#include "ir_rx.h"
#include "ir_tx.h"

#include "lcd_demo.h"
#include "spiflash.h"

#include "datetime.h"

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
#ifdef _BOARD_C_
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



void processOver();
void initPeripheral();
void mm32_evb_hardware_init();


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
