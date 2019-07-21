////////////////////////////////////////////////////////////////////////////////
#define _STUDY_THREAD_C_
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "EVBoard.h"

#include <rtthread.h>
#include "study_thread.h"



/* 学习函数入口 */
void study_entry(void* parameter)
{
	while (1)
    {
		LED_1_ON();     //LD1输出亮		
		
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
	}   
}


///* 学习函数入口 */
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



///* 学习函数入口 */
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
void keys_entry(void* parameter)
{
    while (1)
    {
//		if(Key1){
//			tog = TRUE;
//		}
//		else{
//			tog = FALSE;
//		}
		
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
    }
}








