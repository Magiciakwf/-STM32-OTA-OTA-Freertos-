#include "key.h"
#include "stm32f10x.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 1. 使能 GPIOE 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    /* 2. 配置 PE4 为上拉输入 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

uint8_t KEY_Read(void)
{
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_RESET)
    {
			
        return 0;   // 按键按下（低电平）
    }
    else
    {
        return 1;   // 按键松开
    }
}
