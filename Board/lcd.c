////////////////////////////////////////////////////////////////////////////////
#define _LCD_C_
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "EVBoard.h"
#include "font.h"   //×Ö·û


////////////////////////////////////////////////////////////////////////////////
void initGPIO_LCD()
{
	GPIO_InitTypeDef GPIO_InitStructure;	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//  ====== LCD_busCtrl ======
	//	pd2		nRST
	GPIO_ResetBits(GPIOD, GPIO_Pin_2);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//	pc11	BLC
	GPIO_ResetBits(GPIOC, GPIO_Pin_11);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
#ifdef MDM2803	
	//	pc12	RS
	GPIO_SetBits(GPIOC,  GPIO_Pin_12);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//	pb12	nCS
	//	pb13	nWR
	//	pb14	nRD
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	lcdCsL();
#else			// MDM2802
	//	pb12	nCS
	//	pb14	nRD
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//	pb13	nWR	/ SCK
	//	pb15	MOSI
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
#endif	
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void lcdCmd(u8 cmd)
{
#ifdef MDM2803	
    lcdRsL();
	GPIO_Write(GPIOC, (GPIO_ReadOutputData(GPIOC) & 0xFF00) |  ( cmd       & 0x00FF));
	GPIO_Write(GPIOB, (GPIO_ReadOutputData(GPIOB) & 0xFF00) |  ((cmd >> 8) & 0x00FF));
	lcdWriteL();
	lcdWriteH();
#else
	#ifndef MM32F103xA	
		u32 crh;
		lcdCsL();
		spiSoftLow();
		spiWrite(cmd);
		lcdCsH();
	#else
		u32 crh;
		lcdCsL();
		spiSoftLow();
		spiWrite(cmd);
		lcdCsH();
		
	#endif	
		
#endif
}

void lcdData(u8 dat)
{	
#ifdef MDM2803	
    lcdRsH();
	GPIO_Write(GPIOC, (GPIO_ReadOutputData(GPIOC) & 0xFF00) |  ( dat       & 0x00FF));
	GPIO_Write(GPIOB, (GPIO_ReadOutputData(GPIOB) & 0xFF00) |  ((dat >> 8) & 0x00FF));
	lcdWriteL();
	lcdWriteH();
#else
	#ifndef MM32F103xA	
		u32 crh;
		lcdCsL();
		spiSoftHigh();
		spiWrite(dat);
		lcdCsH();
	#else
		u32 crh;
		lcdCsL();
		spiWrite(dat | 0x80);
		lcdCsH();
		
	#endif
#endif
}

void lcdData16(u16 dat)
{	
#ifdef MDM2803	
    lcdRsH();
	GPIO_Write(GPIOC, (GPIO_ReadOutputData(GPIOC) & 0xFF00) |  ( dat       & 0x00FF));
	GPIO_Write(GPIOB, (GPIO_ReadOutputData(GPIOB) & 0xFF00) |  ((dat >> 8) & 0x00FF));
	lcdWriteL();
	lcdWriteH();
#else
	#ifndef MM32F103xA	
		u32 crh;
		lcdCsL();
		spiSoftHigh();
		spiWrite(dat >> 8);
		spiSoftHigh();
		spiWrite(dat);
		lcdCsH();
	#else
		u32 crh;
		lcdCsL();
		spiWrite(((dat >> 8) & 0x00ff)| 0x80);
		spiWrite((dat & 0x00ff) | 0x80);
		lcdCsH();
	#endif
#endif
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void lcdSetWindow(u32 x, u32 y, u32 w, u32 h) 
{
    lcdCmd(0x2A);
#ifdef MDM2803
    lcdData(x >> 8);
    lcdData(x & 0xFF);
    lcdData((x + w - 1) >> 8);
    lcdData((x + w - 1) & 0xFF);	
#else
	lcdData16(x);
	lcdData16(x + w - 1);
#endif
	
    lcdCmd(0x2B);
#ifdef MDM2803
    lcdData(y >> 8);
    lcdData(y & 0xFF);
    lcdData((y + h - 1) >> 8);
    lcdData((y + h - 1) & 0xFF);
#else	
	lcdData16(y);
	lcdData16(y + h - 1);
#endif	
}

////////////////////////////////////////////////////////////////////////////////
void lcdRst()
{
 	GPIOD->BRR =   GPIO_Pin_2;
	_delay(20000);
	GPIOD->BSRR =  GPIO_Pin_2;
	_delay(100000);
}

////////////////////////////////////////////////////////////////////////////////
void lcdFillColor(u16 c) 
{
	lcdSetWindow(0, 0, WIDTH,  HEIGHT);
    lcdCmd(0x2c);

#ifdef MDM2803	
	lcdRsH();
	GPIO_Write(GPIOC, (GPIO_ReadOutputData(GPIOC) & 0xFF00) |  ( c       & 0x00FF));
	GPIO_Write(GPIOB, (GPIO_ReadOutputData(GPIOB) & 0xFF00) |  ((c >> 8) & 0x00FF));

	for(u16 i = 0; i < (WIDTH * HEIGHT) / 8  ; i++) {
		wrPulse();
		wrPulse();
		wrPulse();
		wrPulse();
		
		wrPulse();
		wrPulse();
		wrPulse();
		wrPulse();
	}
#else
	for(u16 i = 0; i < (WIDTH * HEIGHT) / 8  ; i++) {
    	lcdData16(c);
    	lcdData16(c);
    	lcdData16(c);
    	lcdData16(c);

		lcdData16(c);
    	lcdData16(c);
    	lcdData16(c);
    	lcdData16(c);
	}
#endif	
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void drawSquare(u16 x, u16 y, u16 w, u16 h, u8 frame, u8 fill)
{
	if (!frame) {
		if (fill)
			drawRec (x, y, w, h, color.c2);
		drawRec (x, 			y, 			w, 	1, color.c1);
		drawRec (x, 			y + h - 1, 	w, 	1, color.c1);
		drawRec (x, 			y, 			1, 	h, color.c1);
		drawRec (x + w - 1, 	y, 			1, 	h, color.c1);
	}
	else {
		drawRec (x, y, w, h, color.c1);
	}
}

////////////////////////////////////////////////////////////////////////////////
void drawShadow(u16 x, u16 y, u16 w, u16 h,u8 frame, u16 c)
{
	drawLine (x + 1,	y + h,  x + w, y + h, c);
	drawLine (x + w, y + 1, 	x + w, y + h, c);
}


////////////////////////////////////////////////////////////////////////////////
void drawFrame(u16 x, u16 y, u16 w, u16 h,u8 frame, u8 sel)
{
	drawSquare(x, y, w, h, frame, FILL);
	
	u16 colour = (!sel) ?  color.c3 :  color.c4;
	drawShadow(x, y, w, h, frame, colour);
}


////
void drawSquare_1(u16 x, u16 y, u16 w, u16 h, u16 c)
{
	drawRec (x, y, w, h, c);
}

void drawFrame_1(u16 x, u16 y, u16 w, u16 h, u16 c)
{
	drawRec (x, 			y, 			w, 	1, c);
	drawRec (x, 			y + h - 1, 	w, 	1, c);
	drawRec (x, 			y, 			1, 	h, c);
	drawRec (x + w - 1, 	y, 			1, 	h, c);
}


////////////////////////////////////////////////////////////////////////////////
void drawDialog(u16 w, u16 h, char* str, Color_Def c)
{
	u16 x = (320 - w) / 2;
	u16 y = (240 - h) / 2;

	drawSquare_1(x, y, w, h, c.back);
	drawFrame_1(x + 6,  y + 6,  w - 12,  h - 12,  c.fore);
	
	text.back = LightGrey;
	putStr(x + 12,  y + 1, 2, 1, str);
}

void clearDialog(u16 w, u16 h, Color_Def c)
{
	u16 x = (320 - w) / 2;
	u16 y = (240 - h) / 2;
	drawSquare_1(x, y, w, h, c.back);
}


////////////////////////////////////////////////////////////////////////////////
void drawButton(u16 x, u16 y, u16 w, u16 h, u8 frame, u8 sel, char* str)
{
	drawFrame(x,  y,  w,  h,  frame,  sel);
	putStr(x + (w -  strlen((char*)str) * 8) / 2,  y + 3, 2, 0, str);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void _delay(u32 n)
{ 
	for(u32 i = 0; i < n; i++);
}

////////////////////////////////////////////////////////////////////////////////
void drawChar_6x8(u16 x, u16 y, u8 bc, u8 chr) 
{
    if (0x1F < chr && chr < 0x90) {
        u16 k = (chr - 0x20) * 8;
        for (u16 i = 0; i < 8; i++) {
            for (u16 j = 0; j < 6; j++) {
                if (Font_6x8_h[k + i] & (0x01<<(7 - j))) {
                    drawPoint(x + j, y + i, text.fore);
                }
                else {
                    if (bc) {
                        drawPoint(x + j, y + i, text.back);
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void drawChar_8x16(u16 x, u16 y, u8 bc, u8 chr) 
{
    if (0x1F < chr && chr < 0x90) {
        u16 k = (chr - 0x20) * 16;
        for (u16 i = 0; i < 16; i++) {
            for (u16 j = 0; j < 8; j++){
              	if (Font_8x16_h[k + i] & 0x01 << (7-j)){
                  	drawPoint(x + j, y + i, text.fore);
              	}
              	else{
                  	if (bc) {
                    	drawPoint(x + j, y + i, text.back);
                  	}
              	}
            }
        }
    }
}

void drawChar_8x12(u16 x, u16 y,  u8 bc, char c) 
{
}
void drawChar_16x24(u16 x, u16 y, u8 bc, char c) 
{
}

////////////////////////////////////////////////////////////////////////////////
void putChar(u16 x, u16 y, u8 font, u8 bc, char c) 
{
    if (c & 0x80) return;
    switch (font) {
        case 0:  /* Font 6 x 8 */
            drawChar_6x8(x, y, bc, c);
            break;
        case 1:  /* Font 6 x 8 */
            drawChar_8x12(x, y, bc, c);
            break;
		case 2:  /* Font 8 x 16 */
            drawChar_8x16(x, y, bc, c);
			break;
        default:
            drawChar_16x24(x, y, bc, c);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
void putStr(u16 x, u16 y, u8 font, u8 bc, char *str) 
{
    while (*str) {
        switch(font) {
        case 0:
          putChar(x, y, font, bc, *str++);
          x += 6;
          break;
        case 1:
          putChar(x, y, font, bc, *str++);
          x += 8;
          break;
		case 2:
          putChar(x, y, font, bc, *str++);
          x += 8;
          break;
		default:
          putChar(x, y, font, bc, *str++);
          x += 16;
          break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void putBmp(u16 x, u16 y, u16 w, u16 h, u16 *ptr) 
{
    lcdSetWindow(x, y, w, h);
    lcdCmd(0x2c);
    
    for(u32 i = 0; i < (w * h); i++){
		lcdData(*ptr++); 
    }
}

//void image_display(u16 x,u16 y, u8* imgx)
//{
//	HEADCOLOR *imginfo;
// 	u8 ifosize = sizeof(HEADCOLOR);
//	imginfo=(HEADCOLOR*)imgx;
// 	putBmp(x, y, imginfo->w , imginfo->h,  (u16*)(imgx + ifosize));
//}

////////////////////////////////////////////////////////////////////////////////
void drawRec (u16 x, u16 y, u16 w, u16 h, u16 c)
{
	lcdSetWindow (x, y, w, h);
	lcdCmd(0x2c);
	
	for (u16 i = 0; i < (w * h); i++){
        lcdData16(c);
	}
}

void drawPoint(u16 x, u16 y, u16 c) 
{
	drawRec(x, y, 1, 1, c);
}



////////////////////////////////////////////////////////////////////////////////
void drawLine (s16 x1, s16 y1, s16 x2, s16 y2, u16 c)
{
	u16 w = (x1 > x2) ? (x1 - x2) : (x2 - x1);
	u16 h = (y1 > y2) ? (y1 - y2) : (y2 - y1);
	
	u16 x = (x1 > x2) ? (x2) : (x1);
	u16 y = (y1 > y2) ? (y2) : (y1);
	
	if (y1 == y2) {
		drawRec(x, y, w, 1, c);
	}
	else if (x1 == x2){
		drawRec(x, y, 1, h, c);
	}
	else if (w >= h) {
		for (u16 i = 0; i < w; i++){
			drawRec(x + i, 
					   i * (y2 - y1 - 1) / w + y1, 
					   1, 
					   1, 
					   c);
		}		
	}
	else {
		for (u16 i = 0; i < h; i ++){
			drawRec(
					   ((y + i) * w ) / h + x,  
					   y + i, 
					   1, 
					   1, 
					   c);
		}			   
	}
}


////////////////////////////////////////////////////////////////////////////////
void drawTab(u16 x, u16 y, u16 w, u16 h, u8 m, u8 n)
{
	for (u8 i = 1; i < m; i++){
		drawLine (x + i * (w / m),   y,  x + i * (w / m),  y + h,  color.c1);
	}
	for (u8 i = 1; i < n; i++){
		drawLine (x,  y + i * (h / n),  x + w,  y + i * (h / n),  color.c1);
	}
	drawSquare(x, y, w, h, SPACE, NUL);
}	
	
void drawTabTitle(u16 x, u16 y, u16 w, u16 h, u8 m, u8 n)
{	
	char str[8];
	strcpy( str, "ADC");
	putStr(175, 165, 0, 1, str);

	
	
	strcpy( str, "IR");
	putStr(253, 165, 0, 1, str);
	
	
	strcpy( str, "UART1");
	putStr(175, 180, 0, 1, str);
	
	strcpy( str, "UART2");
	putStr(243, 180, 0, 1, str);
	

	strcpy( str, "SPI");
	putStr(175, 195, 0, 1, str);
	

	strcpy( str, "I2C");
	putStr(175, 210, 0, 1, str);


	strcpy( str, "CAN");
	putStr(175, 225, 0, 1, str);
}



////////////////////////////////////////////////////////////////////////////////
void drawCircle(u16 x, u16 y, u16 r, u16 c)
{
    s32 D = 3 - (r << 1);	/* Decision Variable */
    u32 CurX = 0;			/* Current X Value */
    u32 CurY = r;			/* Current Y Value */
	
    while (CurX <= CurY) {
        drawRec(x + CurX, y + CurY, 1, 1, c);
        drawRec(x + CurX, y - CurY, 1, 1, c);
        drawRec(x - CurX, y + CurY, 1, 1, c);
        drawRec(x - CurX, y - CurY, 1, 1, c);
        drawRec(x + CurY, y + CurX, 1, 1, c);
        drawRec(x + CurY, y - CurX, 1, 1, c);
        drawRec(x - CurY, y + CurX, 1, 1, c);
        drawRec(x - CurY, y - CurX, 1, 1, c);
        if (D < 0){
            D += (CurX << 2) + 6;
        }
        else {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
}



////////////////////////////////////////////////////////////////////////////////
void lcdSetTextColor (u16 _color) 
{
	text.fore = _color;
}

////////////////////////////////////////////////////////////////////////////////
u16 lcdGetTextColor (void) 
{
	return text.fore;
}

////////////////////////////////////////////////////////////////////////////////
void lcdSetBackColor (u16 _color) 
{
	text.back = _color;
}

////////////////////////////////////////////////////////////////////////////////
u16 lcdGetBackColor (void) 
{
	return text.back;
}

////////////////////////////////////////////////////////////////////////////////
void LCDC_Init_Reg() 
{ 
	lcdCmd(0x28);   //display off
	
    lcdCmd(0xCB);   //Power control A
    lcdData(0x39); 
    lcdData(0x2C); 
    lcdData(0x00); 
    lcdData(0x34); 
    lcdData(0x02); 

    lcdCmd(0xCF);   //Power control B
    lcdData(0x00); 
    lcdData(0xC1); 	//lcdData(0x81); 
    lcdData(0X30); 
    
    lcdCmd(0xED);   //Power on sequence control
    lcdData(0x64); 
    lcdData(0x03); 
    lcdData(0X12); 
    lcdData(0X81); 
    
    lcdCmd(0xE8);   //Driver timing control A
    lcdData(0x85); 
    lcdData(0x10); 
    lcdData(0x7A); 
    
    lcdCmd(0xF7);   //Pump ratio control
    lcdData(0x20); 
    
    lcdCmd(0xEA);   //Driver timing control B
    lcdData(0x00); 
    lcdData(0x00); 
    
    lcdCmd(0xC0);    //Power control 
    lcdData(0x1B);   //VRH[5:0]  1B
    
    lcdCmd(0xC1);    //Power control 
    lcdData(0x01);   //SAP[2:0];BT[3:0] 
    
    lcdCmd(0xC5);    //VCM control 
    lcdData(0x45);   //3F
    lcdData(0x25);   //3C
    
    lcdCmd(0xC7);    //VCM control2 
    lcdData(0XB7);   //b7
    
    lcdCmd(0x36);    // Memory Access Control 
    lcdData(0x28); 
    
    lcdCmd(0x3A);    //Pixel Format Set
    lcdData(0x55); 
    
    lcdCmd(0xB1);    //Frame Rate Control
    lcdData(0x00);   
    lcdData(0x1A); 
    
    lcdCmd(0xB6);    //Display Function Control 
    lcdData(0x0A); 
    lcdData(0x82); 
    
    lcdCmd(0xF2);    //3Gamma Function Disable 
    lcdData(0x00); 
    
    lcdCmd(0x26);    //Gamma curve selected 
    lcdData(0x01); 
    
    lcdCmd(0xE0);    //Set Gamma 
    lcdData(0x0F); 
    lcdData(0x2A); 
    lcdData(0x28); 
    lcdData(0x08); 
    lcdData(0x0E); 
    lcdData(0x08); 
    lcdData(0x54); 
    lcdData(0XA9); 
    lcdData(0x43); 
    lcdData(0x0A); 
    lcdData(0x0F); 
    lcdData(0x00); 
    lcdData(0x00); 
    lcdData(0x00); 
    lcdData(0x00); 
    
    lcdCmd(0xE1);    //Set Gamma 
    lcdData(0x00); 
    lcdData(0x15); 
    lcdData(0x17); 
    lcdData(0x07); 
    lcdData(0x11); 
    lcdData(0x06); 
    lcdData(0x2B); 
    lcdData(0x56); 
    lcdData(0x3C); 
    lcdData(0x05); 
    lcdData(0x10); 
    lcdData(0x0F); 
    lcdData(0x3F); 
    lcdData(0x3F); 
    lcdData(0x0F); 
    
    lcdCmd(0x11);   //Exit Sleep
    _delay(200000);
    lcdCmd(0x29);   //display on
}

////////////////////////////////////////////////////////////////////////////////