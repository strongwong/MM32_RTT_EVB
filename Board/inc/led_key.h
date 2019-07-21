////////////////////////////////////////////////////////////////////////////////
#ifndef __GPIO_H_
#define __GPIO_H_
////////////////////////////////////////////////////////////////////////////////

#define offsetH		50

#define Key1	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define Key2  	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)
#define Key3  	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)

////////////////////////////////////////////////////////////////////////////////
#ifdef _LED_KEY_C_
#define GLOBAL

#else
#define GLOBAL extern
#endif

GLOBAL u8	ledCnt;

GLOBAL u8	fKey0;		// menuOverTime Flag
GLOBAL u32	fKeyCnt;	// menuOverTime Count

GLOBAL u8	fKey1,   fKey2,   fKey3;
GLOBAL u8	fKeyP1,  fKeyP2,  fKeyP3;

GLOBAL u32	Key1Cnt,   Key2Cnt,   Key3Cnt;
GLOBAL u8	Key1CntF,  Key2CntF,  Key3CntF;


#undef GLOBAL
////////////////////////////////////////////////////////////////////////////////


void initGPIO_LED();
void initGPIO_KEY();

void ProcessLED();
void LED_2_ON();
void LED_2_OFF();
void LED_1_ON();
void LED_1_OFF();


void dispButton();
void showButton(u16 x, u16 y, u8 frame, char* str);
u8 eventKey(u8 key, u8* flag);
void dispMenuButton(u8 idx);

void scanKey();


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
