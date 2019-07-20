////////////////////////////////////////////////////////////////////////////////
#define _STUDY_THREAD_
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "EVBoard.h"

#include <rtthread.h>
#include "study_thread.h"

/* 线程1入口 */
void study_entry(void* parameter)
{
	while (1)
    {
        LED_0_OFF();    //LD2输出灭
        LED_1_ON();     //LD1输出亮
        rt_thread_delay(RT_TICK_PER_SECOND / 1);
	}   /* 死循环 */
}

/* 线程2入口 */
void thread2_entry(void* parameter)
{
    while (1)
    {
        LED_0_ON();     //LD2输出亮
        LED_1_OFF();    //LD1输出灭
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
        // 休眠1秒 
        // rt_thread_delay(RT_TICK_PER_SECOND);
    }
}







