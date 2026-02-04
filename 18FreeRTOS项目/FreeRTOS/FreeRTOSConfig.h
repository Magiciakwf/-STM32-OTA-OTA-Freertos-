/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */
/* 如果你是 STM32F1 系列 (标准库) */
#include "stm32f10x.h"


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* 1. 中断函数映射 (必须，否则无法运行) */
#define vPortSVCHandler         SVC_Handler
#define xPortPendSVHandler      PendSV_Handler
#define xPortSysTickHandler     SysTick_Handler/* 1. 中断函数映射 (必须，否则无法运行) */
#define vPortSVCHandler         SVC_Handler
#define xPortPendSVHandler      PendSV_Handler
#define xPortSysTickHandler     SysTick_Handler

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( ( unsigned long ) 72000000 )
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 5 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 17 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 16 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1

/* 定义 Cortex-M 中断优先级位数 */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS       __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS       4
#endif

#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define INCLUDE_uxTaskGetStackHighWaterMark   1
#define configUSE_MUTEXES    1


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define INCLUDE_xTaskGetSchedulerState  1

//互斥量
#define configUSE_MUTEXES                 1
#define configUSE_RECURSIVE_MUTEXES       1

//计数信号量
#define configUSE_COUNTING_SEMAPHORES     1

//软件定时器
#define configUSE_TIMERS                  1
#define configTIMER_TASK_PRIORITY         ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH          10
#define configTIMER_TASK_STACK_DEPTH      ( configMINIMAL_STACK_SIZE * 2 )



//
/* =============================================================
 * FreeRTOS 中断优先级配置 (修复版)
 * ============================================================= */

/* 1. 确保 configPRIO_BITS 被定义 (防呆措施) */
#ifndef configPRIO_BITS
    #ifdef __NVIC_PRIO_BITS
        #define configPRIO_BITS   __NVIC_PRIO_BITS
    #else
        #define configPRIO_BITS   4  /* STM32F1/F4 标准为 4 位 */
    #endif
#endif

/* 2. 最低优先级 (15) - 给人类看的 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15

/* 3. 系统调用管理的中断阈值 (5) - 给人类看的
 * 优先级 0-4 的中断不会被 FreeRTOS 屏蔽 (零延迟)
 * 优先级 5-15 的中断归 FreeRTOS 管理
 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

/* 4. 内核中断优先级 (硬件值: 自动左移计算) */
#define configKERNEL_INTERRUPT_PRIORITY      ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* 5. 系统调用最大优先级 (硬件值: 自动左移计算) */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* 6. 别名兼容 */
#define configMAX_API_CALL_INTERRUPT_PRIORITY configMAX_SYSCALL_INTERRUPT_PRIORITY

#endif /* FREERTOS_CONFIG_H */

