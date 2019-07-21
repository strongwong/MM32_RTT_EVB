////////////////////////////////////////////////////////////////////////////////
#define _ADC_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "EVBoard.h"


////////////////////////////////////////////////////////////////////////////////
void initGPIO_ADC()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void NVIC_ADC()
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}


////////////////////////////////////////////////////////////////////////////////
void initADC()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(ADC1->ADDATA);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC1Value[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 2;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
    DMA_Cmd(DMA1_Channel1, ENABLE);

	
	
////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitTypeDef ADC_InitStructure;
	
    ADC_InitStructure.ADC_Mode = ADC_Mode_Continuous_Scan;
    ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_16;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, 0);    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, 0);    
    
	ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
	ADC1->ADCFG |= 0x04;
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


////////////////////////////////////////////////////////////////////////////////
void AdcTemp()
{
	// 25deg temp reference value at 0x1FFF_F7F4
	static float temp;

	u16 *p;
	p = (u16*)0x1FFFF7F4;
	
	temp =  ((*p == 0xffff) || (*p == 0x0000) || (*p < 1700) || (*p > 1900)) 
		?  1800  : *p;	
		
	adcVol += ADC1Value[0];
	adcTmp += ADC1Value[1];
	
	if (rfCnt.adc++ >= 40) {
		rfCnt.adc = 0;
		adcVolt = ((adcVol / 40) * 30 + adcVolt * 70) / 100;  
		if (adcVolt > 4096)  adcVolt = 0;
		adcVol = 0;
		rf.adc = TRUE;
	} 
	
	if (rfCnt.temp++ >= 400) {
		rfCnt.temp = 0;
		temp = ((adcTmp / 400) * 10 + temp * 90) / 100;  
		adcTemp = (10 * (temp - 1800) / 43) + 25;
		adcTmp = 0;
		rf.temp = TRUE;
    } 
}















