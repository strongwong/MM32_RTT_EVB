////////////////////////////////////////////////////////////////////////////////
#define _TIMERS_C_
////////////////////////////////////////////////////////////////////////////////

#include "EVBoard.h"

// arr: 自动重装载值
// psc：时钟预分频数 
// 这里使用 TIM2
void init_TIM(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		//时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; 					//下一个周期自动从装载值 计数到 5000 = 500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 					//时钟频率的预分频值  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 				//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);					// 使能 TIM 中断
}

void NVIC_TIM()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  			//TIM 中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  	//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  		//从优先级 3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM2, ENABLE);  									//TIM 使能
}

//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
   //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
//	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 				//TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	TIM_TimeBaseStructure.TIM_Period = arr; 				//下一个更新事件自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 				//设置TIMx时钟频率预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 			//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);	

 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				//选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; 								//设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM1, &TIM_OCInitStructure); 
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);					//MOE 主输出使能
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  	//CH1预装载使能	 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); 				//使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM1, ENABLE);  							//使能TIM1
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  ); 
		LED1=!LED1;
	}
}