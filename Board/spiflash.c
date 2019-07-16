//////////////////////////////////////////////////////////////////////////////////	 
#define _SPIFLASH_C_
//////////////////////////////////////////////////////////////////////////////////	 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "HAL_device.h"
#include "main.h" 
#include "spi.h" 
#include "spiflash.h" 

////////////////////////////////////////////////////////////////////////////////
void InitSPIFLASH(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				// DMA数据宽度
	SPI_InitStructure.SPI_DataWidth = SPI_DataWidth_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;    					// mode0 SPI_CPOL_Low, SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  					// mode3 SPI_CPOL_High,SPI_CPHA_2Edge
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = 2;				//spi_baud_div;			// SPI最高40MHz，72M最多2分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	
#ifdef MM32F103xA	
	SPI_InitStructure.SPI_length = 8;
#endif	
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE); 											// Enables the specified SPI peripheral SPI使能、主机模式 8位数据模式 SPI 的波特率
	SPIM_TXEn(SPI1);
	SPIM_RXEn(SPI1);	
}

u8 checkSPIFlashId(void)
{
	SPIM_ReadID(SPI1, &spiId[0]);
	return ((spiId[0] != 0x00) | (spiId[1] != 0x00) | (spiId[2] != 0x00)) 
		&  ((spiId[0] != 0xff) | (spiId[1] != 0xff) | (spiId[2] != 0xff)) ;
}	


void initGPIO_SPI(SPI_TypeDef* SPIx)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	if(SPIx==SPI1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  //SPI1 clk enable
		SPIM_CSHigh(SPIx);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;   		//spi1_cs  pa4
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;   		//spi1_sck  pa5
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// 推免复用输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;   		//spi1_mosi  pa7
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// 推免复用输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;  		//spi1_miso  pa6
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		//上拉输入   
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	}
	
	if(SPIx==SPI2){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//SPI2 clk enable
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;   		//spi2_cs  pb12
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	// 推免复用输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;   		//spi2_sck  pb13
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// 推免复用输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;   		//spi2_mosi  pb15
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// 推免复用输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;  		//spi2_miso  pb14
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		//上拉输入   
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	}
}




