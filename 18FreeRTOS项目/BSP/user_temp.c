#include "user_temp.h"

static float vol = 0;
static float vol_tmp = 0;
static float temperature = 0;
static uint16_t DMA_Buf[Buf_Size];
static uint16_t count = 0;
//根据 C 语言标准，静态变量（Static variables）
// 和 全局变量 如果没有显式初始化，编译器会自动将它们初始化为 0（对于指针则是 NULL）。

void user_temptask(void *pvParameters)
{
	if(count%2 == 0)
	{
		vol_tmp = DMA_Buf[count]/4096.0f*3.3f;
		temperature = (1.43f - vol_tmp) / 0.0043f + 25.0f;
		Serial_Printf("Temp Value is %.1f\r\n",temperature);
	}
	else
	{
		vol = DMA_Buf[count]/4096.0f*3.3f;
		Serial_Printf("VCC Value is %.1f\r\n",vol);
	}  
	count++;
	count%=127;    
}