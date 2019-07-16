////////////////////////////////////////////////////////////////////////////////
#define _RTC_C_
////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include "HAL_device.h"
#include "main.h" 
#include "datetime.h"
#include "rtc.h" 


////////////////////////////////////////////////////////////////////////////////
void NVIC_RTC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_SEC);
	}
}

////////////////////////////////////////////////////////////////////////////////
void initRTC(void)
{
	PWR_BackupAccessCmd(ENABLE);
	//BKP_DeInit();
	RCC_LSEConfig(RCC_LSE_ON);
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	RTC_WaitForLastTask();
}


////////////////////////////////////////////////////////////////////////////////
void checkRTC(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
//	PWR_BackupAccessCmd(DISABLE);
	PWR_BackupAccessCmd(ENABLE);

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)  {
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
		
		PWR_BackupAccessCmd(DISABLE);
		
		initRTC();
	}
	else {
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
			RTC_WaitForSynchro(); 
		
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
	}
	BKP_TamperPinCmd(DISABLE); 
	BKP_ITConfig(DISABLE);

//	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	RCC_ClearFlag();
	
	timeSel = 0;
	
}


////////////////////////////////////////////////////////////////////////////////




