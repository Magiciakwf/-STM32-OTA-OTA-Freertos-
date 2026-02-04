#include "iap.h"

// 定义一个函数指针类型，这个函数没有参数，也没有返回值
typedef  void (*pFunction)(void);

void Jump_To_App(uint32_t AppAddr)
{
    pFunction JumpToApplication;
    uint32_t JumpAddress;

    // 【步骤1】检查栈顶地址是否合法
    // STM32F103ZET6 的 RAM 起始地址是 0x20000000
    // 我们检查 AppAddr 所在位置的第一个字（存放的是栈顶指针 SP），看它是否在 RAM 范围内
    if (((*(__IO uint32_t*)AppAddr) & 0x2FFE0000 ) == 0x20000000)
    {
        // 【步骤2】获取复位中断（Reset_Handler）的地址
        // 在中断向量表中，偏移量为 4 的位置存放的是 Reset_Handler 的地址
        JumpAddress = *(__IO uint32_t*) (AppAddr + 4);

        // 【步骤3】将地址强转为函数指针
        JumpToApplication = (pFunction) JumpAddress;

        // 【步骤4】初始化 APP 的堆栈指针 (MSP)
        // 这一步非常重要！必须把 MSP 指向 APP 定义的栈顶，而不是继续用 Bootloader 的栈
        __set_MSP(*(__IO uint32_t*) AppAddr);

        // 【步骤5】跳转！(实际上是执行了 APP 的 Reset_Handler)
        JumpToApplication();
    }
}

void DeInit_Peripherals(void)
{
		int i;
    // 1. 关闭所有中断
    __disable_irq(); 
    
    // 2. 关闭 SysTick (滴答定时器) - 必须关！
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    // 3. 复位所有用过的硬件 (HAL库有这个便利函数)
//    HAL_DeInit(); 
    
    // 4. 清除所有中断挂起位 (防止积压的中断一开机就爆发)
    // 循环清除中断挂起
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i]=0xFFFFFFFF;
        NVIC->ICPR[i]=0xFFFFFFFF;
    }
    
    // 5. 重新开启中断 (交给 APP 去处理)
    __enable_irq();
}