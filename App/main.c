////////////////////////////////////////////////////////////////////////////////
#define _MAIN_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "EVBoard.h"

#include <rtthread.h>
#include "study_thread.h"


int main(void)
{	
	rt_err_t result;
	
	initPeripheral();
	BootShow();
	
    // 初始化 study 
    result = rt_thread_init(&study, "study",        /* 线程名：study */
        study_entry, RT_NULL,                     	/* 线程的入口是 study_entry */
        &study_stack[0], sizeof(study_stack),   	/* 线程栈是study_stack */
        THREAD_PRIORITY, 10);
    if (result == RT_EOK)                           /* 如果返回正确，启动线程1 */
        rt_thread_startup(&study);

    // 初始化 key 处理程序
    result = rt_thread_init(&keys, "keys",         /* 线程名：keys */
        keys_entry, RT_NULL,                     /* 线程的入口是keys_entry */
        &keys_stack[0], sizeof(keys_stack),   /* 线程栈是keys_stack */
        THREAD_PRIORITY - 1, 10);
    if (result == RT_EOK)                           /* 如果返回正确，启动线程2 */
        rt_thread_startup(&keys);
    return 0;
}
