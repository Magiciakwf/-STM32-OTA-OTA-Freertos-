#include "led.h"
#include "main.h"


void Led_Init(void)
{
	GPIO_InitTypeDef Led_InitTypeDef;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	Led_InitTypeDef.GPIO_Mode = GPIO_Mode_Out_PP;
	Led_InitTypeDef.GPIO_Pin = GPIO_Pin_5;
	Led_InitTypeDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&Led_InitTypeDef);
}