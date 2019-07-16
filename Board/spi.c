//////////////////////////////////////////////////////////////////////////////////	 
#define _SPI_C_
//////////////////////////////////////////////////////////////////////////////////	 

#include <stdio.h>
#include "sys.h"
#include "HAL_device.h"

#include "spi.h"
#include "uart.h"	  




extern void DataCompare(u8 *p1, u8 *p2,u16 count);
/********************************************************************************************************
**函数信息 ：SPIM_CSLow(SPI_TypeDef* SPIx)                      
**功能描述 :为选定的SPI 软件重置内部NSS 管脚 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_CSLow(SPI_TypeDef* SPIx)
{
	//Spi cs assign to this pin,select
	if(SPIx==SPI1) 	{	
		GPIO_ResetBits( GPIOA, GPIO_Pin_4 );
	}
	else {
		GPIO_ResetBits( GPIOB, GPIO_Pin_12 );
	}
}

/********************************************************************************************************
**函数信息 ：SPIM_CSHigh(SPI_TypeDef* SPIx)                      
**功能描述 :为选定的SPI 软件配置内部NSS 管脚 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_CSHigh(SPI_TypeDef* SPIx)
{
	//Spi cs release
	if(SPIx==SPI1){	
		GPIO_SetBits( GPIOA, GPIO_Pin_4 );
	}
	else{
		GPIO_SetBits( GPIOB, GPIO_Pin_12 );
	}
}

/********************************************************************************************************
**函数信息 ：SPIM_TXEn(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_TXEn(SPI_TypeDef* SPIx)
{
	//Transmit Enable bit TXEN
	SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
}

/********************************************************************************************************
**函数信息 ：SPIM_TXDisable(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_TXDisable(SPI_TypeDef* SPIx)
{
	//disable TXEN
	SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Tx);
}

/********************************************************************************************************
**函数信息 ：SPIM_RXEn(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_RXEn(SPI_TypeDef* SPIx)
{
	//enable RXEN
	SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
}

/********************************************************************************************************
**函数信息 ：SPIM_RXDisable(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_RXDisable(SPI_TypeDef* SPIx)
{
	//disable RXEN
	SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Rx);
}


/********************************************************************************************************
**函数信息 ：SPIMReadWriteByte(SPI_TypeDef* SPIx,u8 tx_data)                    
**功能描述 : 通过外设 SPIx 收发数据 ,用于全双工模式(同时收发)
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2  ;  tx_data
**输出参数 ：无
********************************************************************************************************/
u16 SPIMReadWriteByte(SPI_TypeDef* SPIx,u8 tx_data)
{
	SPI_SendData(SPIx, tx_data);	
	while (1){
		if(SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL))	{
			return SPI_ReceiveData(SPIx);
		}
	}
}






/********************************************************************************************************
**函数信息 ：SPIM_Init(SPI_TypeDef* SPIx, unsigned short spi_baud_div)            
**功能描述 :可修改参数初始化SPI
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2  ;spi_baud_div
**输出参数 ：无
********************************************************************************************************/
void SPIM_Init(SPI_TypeDef* SPIx,unsigned short spi_baud_div)
{
	SPI_InitTypeDef SPI_InitStructure;
	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				// DMA数据宽度
	SPI_InitStructure.SPI_DataWidth = SPI_DataWidth_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;    					// mode0 SPI_CPOL_Low, SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  					// mode3 SPI_CPOL_High,SPI_CPHA_2Edge
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = spi_baud_div;			// SPI最高40MHz，72M最多2分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
#ifdef MM32F103xA	
	SPI_InitStructure.SPI_length = 8;
#endif	
	SPI_Init(SPIx, &SPI_InitStructure);
	
	SPI_Cmd(SPIx, ENABLE); 											// Enables the specified SPI peripheral SPI使能、主机模式 8位数据模式 SPI 的波特率
	SPIM_TXEn(SPIx);
	SPIM_RXEn(SPIx);	
}

/********************************************************************************************************
**函数信息 ：SPIM_ReadID(SPI_TypeDef* SPIx)                   
**功能描述 :读取ID 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_ReadID(SPI_TypeDef* SPIx, u8* id)
{
	u16 i;
	
	SPIM_CSLow(SPIx);								//Spi cs assign to this pin,select
	SPIMReadWriteByte( SPIx,RDID);				
	
	for(i=0;i<3;i++){
		*(id + i) = SPIMReadWriteByte( SPIx,0x01);
//		printf("temp=0x%x\r\n",temp);		
	}
	
	SPIM_CSHigh(SPIx);   							//Spi cs release
}

/********************************************************************************************************
**函数信息 ：SPIM_WriteEnable(SPI_TypeDef* SPIx)                  
**功能描述 :写数据使能
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_WriteEnable(SPI_TypeDef* SPIx)
{
	SPIM_CSLow(SPIx); 								//Spi cs assign to this pin,select
	SPIMReadWriteByte( SPIx,WREN);	
	SPIM_CSHigh(SPIx); 								//Spi cs release
}

/********************************************************************************************************
**函数信息 ：SSPIM_checkStatus(SPI_TypeDef* SPIx)                 
**功能描述 :检查数据发送是否正确
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_checkStatus(SPI_TypeDef* SPIx)
{
	u8 temp;
	SPIM_CSLow(SPIx); 								//Spi cs assign to this pin,select
	SPIMReadWriteByte( SPIx,RDSR);	
	while(1){
		temp = SPIMReadWriteByte( SPIx,0x00);	
		if((temp&0x01)==0x0)
			break;
	}
	SPIM_CSHigh(SPIx);  							//Spi cs release
}

/********************************************************************************************************
**函数信息 ：SPIM_WriteEnable(SPI_TypeDef* SPIx)                  
**功能描述 :写数据失能
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_WriteDisable(SPI_TypeDef* SPIx)
{	
	SPIM_CSLow(SPIx);
	SPIMReadWriteByte( SPIx,WRDI);
	SPIM_CSHigh(SPIx);
}

/********************************************************************************************************
**函数信息 ：SPIM_PageRead(SPI_TypeDef* SPIx,u32 address,u8 *p,u16 number)               
**功能描述 :读取256页数据
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2  ;  address  ;*p ;number
**输出参数 ：无
********************************************************************************************************/
void SPIM_PageRead(SPI_TypeDef* SPIx,u32 address,u8 *p,u16 number)  //page = 256 bytes
{
	u8 addr0,addr1,addr2;
	u16 i;
	address = address&0xffffff00; 					//page address
	addr0 = (u8)(address>>16);
	addr1 = (u8)(address>>8);
	addr2 = (u8)address;
	
	SPIM_CSLow(SPIx); 								//Spi cs assign to this pin,select
	
	SPIMReadWriteByte( SPIx,READ);
	SPIMReadWriteByte( SPIx,addr0);
	SPIMReadWriteByte( SPIx,addr1);
	SPIMReadWriteByte( SPIx,addr2);
	
	for(i=0;i<256;i++) {
		rxtmpdata[i] = SPIMReadWriteByte(SPIx,0x00);
	}
	
	SPIM_CSHigh(SPIx);  							//Spi cs release
}

/********************************************************************************************************
**函数信息 ：SPIM_PageProgram(SPI_TypeDef* SPIx,u32 address,u8 *p,u16 number)              
**功能描述 :发送256页数据
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2  ;  address;*p;number
**输出参数 ：无
********************************************************************************************************/
void SPIM_PageProgram(SPI_TypeDef* SPIx,u32 address,u8 *p,u16 number)
{
	u16 j;
	u8 addr0,addr1,addr2;
	address = address&0xffffff00; 					//page address
	addr0 = (u8)(address>>16);
	addr1 = (u8)(address>>8);
	addr2 = (u8)address;
	
	SPIM_WriteEnable(SPIx);
	SPIM_CSLow(SPIx);  								//Spi cs assign to this pin,select
	SPIMReadWriteByte(SPIx,PP);
	SPIMReadWriteByte(SPIx,addr0);
	SPIMReadWriteByte(SPIx,addr1);
	SPIMReadWriteByte(SPIx,addr2);
	for(j=0;j<number;j++){SPIMReadWriteByte(SPIx,*(p++));
	}
	
	SPIM_CSHigh(SPIx);  							//Spi cs release
	
	SPIM_checkStatus(SPIx);
}

/********************************************************************************************************
**函数信息 ：SPIM_SectorErase(SPI_TypeDef* SPIx)             
**功能描述 :擦除数据
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输入参数 ：unsigned long address,删除指定sector的地址 each sector = 64Kbytes
**输出参数 ：无
********************************************************************************************************/
void SPIM_SectorErase(SPI_TypeDef* SPIx,u32 address)
{
	u8 addr0,addr1,addr2;
	address = address & 0xffff0000;
	addr0 = ((u8)(address>>16))&0xff;
	addr1 = ((u8)(address>>8))&0xff;
	addr2 = ((u8)address)&0xff;
	
	SPIM_WriteEnable(SPIx);
	
	SPIM_CSLow(SPIx);  								//Spi cs assign to this pin,select
	
	SPIMReadWriteByte(SPIx,SE);
	SPIMReadWriteByte(SPIx,addr0);
	SPIMReadWriteByte(SPIx,addr1);
	SPIMReadWriteByte(SPIx,addr2);
	SPIM_CSHigh(SPIx);  							//Spi cs release
	
	SPIM_checkStatus(SPIx);
}

/********************************************************************************************************
**函数信息 ：SPIM_BlockErase(SPI_TypeDef* SPIx)             
**功能描述 :擦除数据
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_BlockErase(SPI_TypeDef* SPIx)
{
	SPIM_WriteEnable(SPIx);
	
	SPIM_CSLow(SPIx);  								//Spi cs assign to this pin,select
	
	SPIMReadWriteByte( SPIx,BE);	
	
	SPIM_CSHigh(SPIx);  							//Spi cs release
	
	SPIM_checkStatus(SPIx);
}
/********************************************************************************************************
**函数信息 ：SPIM_Close(SPI_TypeDef* SPIx)             
**功能描述 : CS信号拉高，停止通信
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_Close(SPI_TypeDef* SPIx)
{
	SPIM_CSHigh(SPIx);  							//Spi cs release
	SPIMReadWriteByte(SPIx,0x01);
}

/********************************************************************************************************
**函数信息 ：SPIM_Test(SPI_TypeDef* SPIx)          
**功能描述 :测试程序，使用串口打印256页数据
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
//void SPIM_Test(SPI_TypeDef* SPIx)
//{
//	u16 i;
//	u8 id[3];
//	for(i = 0; i < 256; i++){
//		tmpdata[i] = i*2;
//	}
//	
//	printf("spi2 test\r\n");
//	
//	SPIM_Init(SPIx,0x2);						//2分频，36MHz
//	
//	SPIM_ReadID(SPIx, &id[0]);
//	
//	SPIM_SectorErase(SPIx, 0);
//	
//	SPIM_PageProgram(SPIx,256,tmpdata,256);
//	
//	for(i = 0; i < 256; i++){
//		rxtmpdata[i] = 0x0;
//	}
//	
//	SPIM_PageRead(SPIx,256,rxtmpdata,256); 		//把写进去的一页256字节读出来
//	
//	for(i = 0; i < 10; i++){
//		printf("rx[%d]=0x%x\r\n",i,rxtmpdata[i]);
//	}
//	printf("spi2 test over\r\n");
//	
//}

