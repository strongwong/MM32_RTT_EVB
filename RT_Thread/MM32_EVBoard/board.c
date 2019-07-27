/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2019-07-16     strongwong   porting MM32
 */
////////////////////////////////////////////////////////////////////////////////
#define _BOARD_C_
////////////////////////////////////////////////////////////////////////////////

#include <rthw.h>
#include <rtthread.h>

#include <string.h>
#include "EVBoard.h"


////////////////////////////////////////////////////////////////////////////////

extern unsigned int SystemCoreClock;
// rtthread tick configuration
// 1. include header files
// 2. configure rtos tick and interrupt
// 3. add tick interrupt handler

// rtthread tick configuration
// 1. include some header file as need
//#include <your_header_file.h>

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define HEAP_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define HEAP_BEGIN    (&__bss_end)
#endif

#define SRAM_SIZE         20
#define SRAM_END          (0x20000000 + SRAM_SIZE * 1024)


/**
 * This function will initial MM32 board.
 */
void rt_hw_board_init()
{    
	// rtthread tick configuration
	// 2. Configure rtos tick and interrupt
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	//SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init((void*)HEAP_BEGIN, (void*)SRAM_END);
#endif

#ifdef  MM32_EVB
	mm32_evb_hardware_init();		// MM32_EVB all hardware init
#endif
}

// rtthread tick configuration
// 3. add tick interrupt handler 
 void SysTick_Handler(void)
 {
 	/* enter interrupt */
 	rt_interrupt_enter();
 
 	rt_tick_increase();
 
 	/* leave interrupt */
 	rt_interrupt_leave();
 }

////////////////////////////////////////////////////////////////////////////////
void initPara()
{
	testStart = FALSE;

	memset(&rf, 0,  sizeof(rf));
	memset(&rfCnt, 0,  sizeof(rfCnt));
	memset(&rfOverCnt, 0,  sizeof(rfOverCnt));
	
	memset(&uart1RxBuf, 0, sizeof(uart1RxBuf));
	memset(&uart2RxBuf, 0, sizeof(uart2RxBuf));
	
	memset(&irBuf, 0, sizeof(irBuf));
	memset(&irDat, 0, sizeof(irDat));
	
	irp = 0;

	menuCnt = 0;

	adcVol = 0;
	adcTmp = 0;

	canTx = 0;
	uartTx1 = 0;
	uartTx2 = 0;
	
	fKey1 = fKey2 = fKey3 = FALSE;
	Key1Cnt = Key2Cnt = Key3Cnt = 0;
	Key1CntF = Key2CntF = Key3CntF = FALSE;

	fKey0 = FALSE;		// menuOverTime Flag
	fKeyCnt = 0;		// menuOverTime Count
	
	
	
	drawBlockCnt = 0;
	ledCnt = 0;
	
	irTxCnt = 0;
	irTxF = FALSE;
	
	irOverCnt = 0;
	irOver = TRUE;;
}
////////////////////////////////////////////////////////////////////////////////
void initPeripheral(){
	InitSPIFLASH();
	initLCD();
	initADC();
	initCAN();
	initUART();
	initI2C();
	initIR_Rx();
}

////////////////////////////////////////////////////////////////////////////////
void initNVIC(){
	NVIC_RTC();
	NVIC_I2C(I2C2);
	NVIC_CAN();
	NVIC_UART();
}

////////////////////////////////////////////////////////////////////////////////
void initGPIO(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
						   RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_GPIOC |
						   RCC_APB2Periph_GPIOD |
						   RCC_APB2Periph_AFIO,
						   ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	initGPIO_UART();
	initGPIO_SPI(SPI1);
	initGPIO_LED();
	initGPIO_KEY();
	initGPIO_ADC();
	initGPIO_CAN();
	initGPIO_I2C(I2C2);
	initGPIO_LCD();
	initGPIO_IR();
}

////////////////////////////////////////////////////////////////////////////////
void initRCC(void){
	ErrorStatus HSEStartUpStatus;

	RCC_DeInit(); 							 						// RCC system reset(for debug purpose)
	RCC_HSEConfig(RCC_HSE_ON);  									// Enable HSE //
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); 					// Wait till HSE is ready
	for(u32 i = 0; i < 100000; i++); 
  
	if(HSEStartUpStatus == SUCCESS)	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);    	// Enable Prefetch Buffer
		FLASH_SetLatency(FLASH_Latency_2);    						// Flash 2 wait state
		RCC_HCLKConfig(RCC_SYSCLK_Div1);    						// HCLK = SYSCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);    							// PCLK2 = HCLK
		RCC_PCLK1Config(RCC_HCLK_Div1);    							// PCLK1 = HCLK
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);					    	// ADCCLK = PCLK2/6
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);		// PLLCLK = 8MHz * 9 = 72 MHz
		RCC_PLLCmd(ENABLE);					    					// Enable PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}		// Wait till PLL is ready
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);					// Select PLL as system clock source
		while(RCC_GetSYSCLKSource() != 0x08){}				    	// Wait till PLL is used as system clock source
	}
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);					// Set the Vector Table base address at 0x08000000
}

////////////////////////////////////////////////////////////////////////////////
void processOver()
{
	if (menuCnt > 0){
		if (++fKeyCnt > 5000) {
			fKey0 = TRUE;		// set menuOverTime Flag
			fKeyCnt = 0;		// clear menuOverTime Count
		}
	}
	
	if ((rfOverCnt.i2c++ > 1200)  && (!rf.i2c)){
		rfOverCnt.i2c = 0;
		error.i2c = TRUE;
		rf.i2c = TRUE;
	}
	if ((rfOverCnt.can++ > 1200) && (!rf.can)){
		rfOverCnt.can = 0;
		error.can = TRUE;
		rf.can = TRUE;
	}
	if ((rfOverCnt.uart1++ > 1200) && (!rf.uart1)){
		rfOverCnt.uart1 = 0;
		error.uart1 = TRUE;
		rf.uart1 = TRUE;
		
	}
	if ((rfOverCnt.uart2++ > 1200) && (!rf.uart2)){
		rfOverCnt.uart2 = 0;
		error.uart2 = TRUE;
		rf.uart2 = TRUE;
	}
	
	if ((rfOverCnt.ir++ > 2000) && (!rf.ir)){
		rfOverCnt.ir = 0;
		irBlock = FALSE;
		error.ir = TRUE;
		rf.ir = TRUE;
	}

	irNull();
}

void mm32_evb_hardware_init(){
//	initPara();
	initRCC();
	checkRTC();
	initGPIO();
	initNVIC();
//	processOver();
	
}
