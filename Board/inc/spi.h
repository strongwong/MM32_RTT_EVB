
////////////////////////////////////////////////////////////////////////////////
#ifndef __SPI_H__
#define __SPI_H__
////////////////////////////////////////////////////////////////////////////////

#define READ        	0x03
#define FAST_READ   	0x0B
#define RDID        	0x9F
#define WREN 			0x06
#define WRDI 			0x04
#define SE 				0xD8
#define BE 				0xC7
#define PP 				0x02
#define RDSR 			0x05
#define WRSR 			0x01
#define DP 				0xB9
#define RES 			0xAB

////////////////////////////////////////////////////////////////////////////////
#ifdef _SPI_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL u8 tmpdata[256];
GLOBAL u8 tmpdata1[256];
GLOBAL u8 rxtmpdata[256];
GLOBAL u8 spi0_rx_flag;
GLOBAL u8 spi0_tx_flag;


#undef GLOBAL
////////////////////////////////////////////////////////////////////////////////



//extern void DataCompare(u8 *p1, u8 *p2,u16 count);

void SPIM_CSLow(SPI_TypeDef* SPIx);
void SPIM_CSHigh(SPI_TypeDef* SPIx);
void SPIM_TXEn(SPI_TypeDef* SPIx);
void SPIM_TXDisable(SPI_TypeDef* SPIx);
void SPIM_RXEn(SPI_TypeDef* SPIx);
void SPIM_RXDisable(SPI_TypeDef* SPIx);
u16 SPIMReadWriteByte(SPI_TypeDef* SPIx,u8 tx_data);
void SPIM_Init(SPI_TypeDef* SPIx,u16 spi_baud_div);
void SPIM_ReadID(SPI_TypeDef* SPIx, u8* id);
void SPIM_WriteEnable(SPI_TypeDef* SPIx);
void SPIM_checkStatus(SPI_TypeDef* SPIx);
void SPIM_WriteDisable(SPI_TypeDef* SPIx);
void SPIM_PageRead(SPI_TypeDef* SPIx,u32 address,u8 *p,u16 number);  //page = 256 bytes
void SPIM_PageProgram(SPI_TypeDef* SPIx,u32 address,u8 *p,u16 number);
void SPIM_SectorErase(SPI_TypeDef* SPIx,u32 address);
void SPIM_BlockErase(SPI_TypeDef* SPIx);
void SPIM_Close(SPI_TypeDef* SPIx);
void SPIM_Test(SPI_TypeDef* SPIx);




////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////

