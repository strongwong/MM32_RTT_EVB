////////////////////////////////////////////////////////////////////////////////
#define _IR_TX_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "EVBoard.h"


////////////////////////////////////////////////////////////////////////////////
void initIR_Tx()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TIMERPERIOD;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* Channel 3 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	
	/* TIM3 counter enable */
	TIM_Cmd(TIM3, ENABLE);
	
	TIM_ITConfig(TIM3, TIM_IT_CC4, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void irTxMark()
{
	TIM_SetCounter(TIM3, 0);
	TIM3->CCR3 = (u16) (((u32) 50 * (TIMERPERIOD - 1)) / 100);
}

////////////////////////////////////////////////////////////////////////////////
void irTxSpace()
{
	TIM3->CCR3 = 0;
}

////////////////////////////////////////////////////////////////////////////////
void closeIrTx()
{
	if (irTxCnt >= 96) {				
		irTxCnt = 0;
		initIR_Rx();
	}
}

////////////////////////////////////////////////////////////////////////////////
void irSendPacket()
{
	static u8 irTxHead = TRUE;
	static u8 irTxHeadCnt = 0;

	static u8 irTxPhase = FALSE;
	static u8 bitIsMark = FALSE;
	
	if (irTxF) {
		if (irTxHead) {
			if (irTxHeadCnt++ < 3) {
				irTxMark();
			}
			else {
				irTxSpace();
				irTxHead = FALSE;
				irTxHeadCnt = 0;
			}
		}
		else {
			if (!irTxPhase) {
				if (!bitIsMark) {
					irTxMark();
					bitIsMark = ((txbuf[irTxCnt / 8] >> (irTxCnt % 8)) & 0x01);
					irTxPhase =  (bitIsMark) ? FALSE : TRUE;
				}
				else {
					irTxMark();
					bitIsMark = FALSE;
					irTxPhase = TRUE;
				}
			}
			else {
				irTxSpace();
				bitIsMark = FALSE;
				irTxPhase = FALSE;
				
				if (++irTxCnt == 96) {
					txbuf[6] = 0;
					txbuf[7]++;
					
					irTxF = FALSE;
					irTxHead = TRUE;
					irTxHeadCnt = 0;
				}
			}
		}
	}
}

