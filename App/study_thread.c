////////////////////////////////////////////////////////////////////////////////
#define _STUDY_THREAD_C_
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "EVBoard.h"

#include <rtthread.h>
#include "study_thread.h"

u16 led0pwmval=0;    
u8 dir=1;

///* 学习函数入口 */
//点亮一个 LED 灯
void study_entry(void* parameter)
{
	while (1)
    {
		LED_2_ON();     //LD1输出亮		
		
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
	}   
}


//让两个 LED 灯轮流点亮
//void study_entry(void* parameter)
//{
//	while (1)
//    {
//		if(tog){
//			LED_2_OFF();    //LD2输出灭
//			LED_1_ON();     //LD1输出亮
//			tog = FALSE;
//		}
//		else
//		{
//			LED_2_ON();     //LD2输出亮
//			LED_1_OFF();    //LD1输出灭
//			tog = TRUE;
//		}		
//		
//        rt_thread_delay(RT_TICK_PER_SECOND / 10);
//	}   
//}



//
//void study_entry(void* parameter)
//{
//	while (1)
//    {
//		if(tog){
//			LED_2_OFF();    //LD2输出灭
//			LED_1_ON();     //LD1输出亮
//		}
//		else
//		{
//			LED_2_ON();     //LD2输出亮
//			LED_1_OFF();    //LD1输出灭
//		}		
//		
////        rt_thread_delay(RT_TICK_PER_SECOND / 1);
//	}   
//}

/* 按键处理函数入口 */
// 获取按键值改变变量
void keys_entry(void* parameter)
{
//    while (1)
//    {
//		if(Key1){
//			tog = TRUE;
//		}
//		else{
//			tog = FALSE;
//		}
//		
//        rt_thread_delay(RT_TICK_PER_SECOND / 10);
//    }
}


//// 定时器
//void timers_entry(void* parameter)
//{
//	init_TIM(4999,9599);
//	NVIC_TIM();
//	while(1)
//	{
////		LED2 = !LED2;
//		rt_thread_delay(RT_TICK_PER_SECOND * 5/ 10);	// 500ms
//	}
//}

// 定时器
void timers_entry(void* parameter)
{	
	TIM1_PWM_Init(899, 0);	//PWM f = 96000/(899+1) = 106KHz
	
	while(1)
	{	 
		rt_thread_delay(RT_TICK_PER_SECOND / 100);	// 100ms
		
		if(dir)
			led0pwmval++;
		else 
			led0pwmval--;	 
		if(led0pwmval > 400)
			dir=0;
		if(led0pwmval==0)
			dir=1;	   					 
		TIM_SetCompare1(TIM1,led0pwmval);
		
	}
}



