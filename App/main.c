////////////////////////////////////////////////////////////////////////////////
#define _MAIN_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "EVBoard.h"

#include <rtthread.h>

#define THREAD_STACK_SIZE	1024
#define THREAD_PRIORITY	5

/* 线程1控制块及线程栈 */
static struct rt_thread thread1;
ALIGN(4)
static rt_uint8_t thread1_stack[THREAD_STACK_SIZE];

/* 线程2控制块及线程栈 */
static struct rt_thread thread2;
ALIGN(4)
static rt_uint8_t thread2_stack[THREAD_STACK_SIZE];

/* 线程1入口 */
static void thread1_entry(void* parameter)
{
	while (1)
    {
        LED_0_OFF();    //LD2输出灭
        LED_1_ON();     //LD1输出亮
        rt_thread_delay(RT_TICK_PER_SECOND / 1);
	}   /* 死循环 */
}

/* 线程2入口 */
static void thread2_entry(void* parameter)
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


int main(void)
{	
	rt_err_t result;
	//delay_init();	    	    //延时函数初始化	  
    initGPIO_LED();		  	    //初始化与LED连接的硬件接口
	
    // 初始化线程1 
    result = rt_thread_init(&thread1, "t1",         /* 线程名：t1 */
        thread1_entry, RT_NULL,                     /* 线程的入口是thread_entry */
        &thread1_stack[0], sizeof(thread1_stack),   /* 线程栈是thread1_stack */
        THREAD_PRIORITY, 10);
    if (result == RT_EOK)                           /* 如果返回正确，启动线程1 */
        rt_thread_startup(&thread1);

    // 初始化线程2
    result = rt_thread_init(&thread2, "t2",         /* 线程名：t2 */
        thread2_entry, RT_NULL,                     /* 线程的入口是thread_entry */
        &thread2_stack[0], sizeof(thread2_stack),   /* 线程栈是thread2_stack */
        THREAD_PRIORITY - 1, 10);
    if (result == RT_EOK)                           /* 如果返回正确，启动线程2 */
        rt_thread_startup(&thread2);
    return 0;
}
