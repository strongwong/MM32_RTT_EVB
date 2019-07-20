////////////////////////////////////////////////////////////////////////////////
#define _MAIN_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "EVBoard.h"

#include <rtthread.h>
#include "study_thread.h"

////////////////////////////////////////////////////////////////////////////////
void initPeripheral()
{
	InitSPIFLASH();
	initLCD();
	initADC();
	initCAN();
	initUART();
	initI2C();
	initIR_Rx();
}

////////////////////////////////////////////////////////////////////////////////
void initNVIC()
{
	NVIC_RTC();
	NVIC_I2C(I2C2);
	NVIC_CAN();
	NVIC_UART();
}

////////////////////////////////////////////////////////////////////////////////
void initGPIO()
{
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
void initRCC(void)
{
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

int main(void)
{	
	rt_err_t result;
	
	initRCC();
	initGPIO();
	//delay_init();	    	    //延时函数初始化	  
    initGPIO_LED();		  	    //初始化与LED连接的硬件接口
	initLCD();					//初始化 LCD 的硬件接口
	BootShow();
	
    // 初始化 study 
    result = rt_thread_init(&study, "study",        /* 线程名：study */
        study_entry, RT_NULL,                     	/* 线程的入口是 study_entry */
        &study_stack[0], sizeof(study_stack),   	/* 线程栈是study_stack */
        THREAD_PRIORITY, 10);
    if (result == RT_EOK)                           /* 如果返回正确，启动线程1 */
        rt_thread_startup(&study);

    // 初始化线程2
    result = rt_thread_init(&thread2, "t2",         /* 线程名：t2 */
        thread2_entry, RT_NULL,                     /* 线程的入口是thread_entry */
        &thread2_stack[0], sizeof(thread2_stack),   /* 线程栈是thread2_stack */
        THREAD_PRIORITY - 1, 10);
    if (result == RT_EOK)                           /* 如果返回正确，启动线程2 */
        rt_thread_startup(&thread2);
    return 0;
}
