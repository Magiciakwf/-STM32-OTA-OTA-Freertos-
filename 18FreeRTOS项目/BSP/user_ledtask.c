#include "user_ledtask.h"

uint8_t led_flag;

void user_ledtask(void *pvParameters)
{
    while(1)
    {
        if(led_flag == 0)
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_5);
            led_flag = 1;
        }
        else
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_5);
            led_flag = 0;
        }
        vTaskDelay(500);
        
    }
}