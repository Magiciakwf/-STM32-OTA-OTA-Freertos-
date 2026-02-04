#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

/* 引入外部声明 */
extern void xPortSysTickHandler(void);

static u32 fac_us = 0;

//// SysTick中断服务函数
//void SysTick_Handler(void)
//{
//    // 如果OS调度器已经启动，则调用OS的心跳处理
//    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
//    {
//        xPortSysTickHandler();
//    }
//}

// 初始化延迟函数
// SYSCLK: 系统时钟频率，单位 MHz (例如 72)
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS
    u32 reload;
#endif

    // 1. 配置 SysTick 时钟源为 HCLK/8
    // 标准库写法: SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    // 或者直接操作寄存器 (和 HAL 效果一样，兼容性更好):
    SysTick->CTRL &= ~(1 << 2); // CLKSOURCE = 0 (External Clock/8)

    // 2. 计算微秒延时的倍乘数
    fac_us = SYSCLK / 8;

#if SYSTEM_SUPPORT_OS
    // 3. 计算重装载值 (供给 FreeRTOS 的节拍)
    // reload = (SYSCLK / 8) * (1000000 / configTICK_RATE_HZ)
    // 化简防止溢出:
    reload = SYSCLK / 8; 
    reload *= 1000000 / configTICK_RATE_HZ;

    // 4. 开启 SysTick 中断 (TICKINT)
    SysTick->CTRL |= 1 << 1;

    // 5. 加载重装载值
    SysTick->LOAD = reload;

    // 6. 开启 SysTick 计数器 (ENABLE)
    SysTick->CTRL |= 1 << 0; 
#endif
}

/**
  * @brief  微秒级延时 (粗略延时，不通过SysTick)
  * @note   在FreeRTOS中，不能去操作SysTick寄存器，否则会破坏OS节拍
  * 这里使用简单的空循环，基于72MHz主频大致估算
  */
void Delay_us(uint32_t xus)
{
    uint32_t i, j;
    // 这个系数需要根据你的编译器优化等级调整
    // 72MHz下，大致的空循环次数
    for(i = 0; i < xus; i++)
    {
        for(j = 0; j < 5; j++) // 这里的5是个经验值，可能需要根据实际微调
        {
            __NOP(); // 空指令
        }
    }
}

/**
  * @brief  毫秒级延时
  * @note   会自动判断是否开启了OS
  */
void Delay_ms(uint32_t xms)
{
    // 情况1：如果操作系统已经运行了，必须用 OS 的延时函数
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        // FreeRTOS的延时，单位是 Tick，一般配置1Tick=1ms
        vTaskDelay(xms); 
    }
    // 情况2：如果在初始化阶段（OS还没开始跑），用微秒空循环死等
    else
    {
        Delay_us(xms * 1000);
    }
}

/**
  * @brief  秒级延时
  */
void Delay_s(uint32_t xs)
{
    while(xs--)
    {
        Delay_ms(1000);
    }
}
