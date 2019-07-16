////////////////////////////////////////////////////////////////////////////////
#define _IR_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "HAL_device.h"
#include "sys.h"


#include "main.h" 
#include "ir_rx.h" 
#include "datetime.h"

////////////////////////////////////////////////////////////////////////////////
void initGPIO_IR()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void NVIC_IR()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void initIR_Rx()
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler =(1440-1);
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/////////////////	
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x03;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_Cmd(TIM3,ENABLE );
	
	NVIC_IR();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void getMarkSpace(u8 *p, u8 *mark, u8 *space, u8 f)
{
	u32 sumMark = 0;
	u32 sumSpace = 0;
	u16 markCnt = 0;
	u16 spaceCnt = 0;
	
	u8 first;
	if (!f) {
		first = *p++;
		while(*p < 255) {
			if ((*p >= (first - first / 3)) && (*p <= (first + first / 3))) {
				sumMark += *p; 
				markCnt++;
			}
			else {
				sumSpace += *p;
				spaceCnt++;
			}
			p++;
		}
		*mark = sumMark / markCnt;
		*space = sumSpace / spaceCnt;
	}
	else {
		first = *p++;
		while(*p < 255) {
			if ((first >= (*p  - *p / 5)) && ((first <= (*p + *p / 5)))) { 
				sumMark += *p; 
				markCnt++;
			}
			else {
				sumSpace += *p;
				spaceCnt++;
			}
			p++;
		}
		*mark = sumMark / markCnt;
		*space = sumSpace / spaceCnt;
		
	}
}

////////////////////////////////////////////////////////////////////////////////
void putMarkSpace(u8 *p, u8 mark, u8 space, u8 f)
{
	while(*p < 255) {
		((*p > (mark - mark / 5)) && (*p < (mark + mark / 5))) ?  (*p++ = mark) :  (*p++ = space); 
	}
}

////////////////////////////////////////////////////////////////////////////////
void irConvert(u8 *p, u8 f)
{
	u8 i = 0; 
	u8 dat = 0;
	u8 ref;
	if (!f) {
		ref = *p++;
		while(*p < 255) {
			for (u8 j = 0; j < 8; j++) {
				dat >>= 1;
				(*p++ == ref) ?  (dat &= ~0x80) : (dat |=  0x80); 
				if (*p == 255)  break;
				p++;
			}
			irDat[i++] = dat;
			dat = 0;
		}
	}
	else {
		ref = *p;
		while(*p < 255) {
			for (u8 j = 0; j < 8; j++) {
				dat >>= 1;
				(*++p == ref) ?  (dat &= ~0x80) : (dat |=  0x80); 
				if (*p == 255) {
					dat >>= (7 - j);
					break;
				}
				p++;
				if (*p == 255) {
					dat >>= (7 - j);
					break;
				}
			}
			irDat[i++] = dat;
			dat = 0;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void irNormalization(u8* p, u8 f)
{
	u8 mark = 0;	u8 space = 0;
	getMarkSpace(p, &mark, &space, f);
	putMarkSpace(p,  mark,  space, f);
}

////////////////////////////////////////////////////////////////////////////////
void irNull()
{
	if (!irOver && (irOverCnt++ >= 9)) {
		irOver = TRUE;	
		TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Falling);
		irBuf[irp] = 255;
		irp = 0;
		
		if (irSyncF && !irBlock) {
			irBlock = TRUE;
			rfOverCnt.ir = 0;
			if (irSync0 && irSync1) {
				irNormalization(&irBuf[0], 0);
				irConvert(&irBuf[0], 0);
			}
			else if (irSync0 && !irSync1) {
				irNormalization(&irBuf[0], 1);
				irConvert(&irBuf[0], 1);
			}
			
			if (strcmp((char*)irDat, "Manley") == 0) {
				if (*(u32*)(irDat + 8) >  230000000)			
					RTC_SetCounter(*(u32*)(irDat + 8));
			}
			error.ir = FALSE;
			rf.ir = TRUE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void TIM3_IRQHandler()
{ 		  
	static u32 _cnt = 0;
	if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET) {
		u32 cnt = TIM_GetCapture4(TIM3);
		TIM_ClearFlag(TIM3, TIM_IT_CC4);
		
		(IR) ? TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Falling)
			: TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Rising);
			
		irOverCnt = 0;
		
		if (irBlock) {
			irOver = TRUE;
			rfOverCnt.ir = 0;
		}
		else if (irOver){
			irOver = FALSE;
			irSync0 = 0;
			irSync1 = 0;
			irSyncF = FALSE;
			memset(&irBuf, 0, sizeof(irBuf));
			memset(&irDat, 0, sizeof(irDat));
		}
		else {
			if (!irSyncF) {
				if ((irSync0 == 0) && ((cnt - _cnt) > 100)) {
					irSync0 = (cnt - _cnt);
					irSync1 = 0;
					irSyncF = FALSE;
				}
				else if (irSync0 && ((cnt - _cnt) > 75) && ((cnt - _cnt) < 120)) {
					irSync1 = (cnt - _cnt);
					irSyncF = TRUE;
				}
				else if (irSync0) {
					irBuf[irp++] = cnt - _cnt;
					irSync1 = 0;
					irSyncF = TRUE;
				}
			}
			else {
				irBuf[irp++] = cnt - _cnt;
				
				if (irp >= 512) {
					irp = 0;
					irSyncF = FALSE;
				}
			}
		}
		_cnt = cnt;
	}
}

////////////////////////////////////////////////////////////////////////////////


