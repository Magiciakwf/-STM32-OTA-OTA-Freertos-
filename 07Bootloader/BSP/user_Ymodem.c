#include "user_Ymodem.h"
#include "flash.h"

#define FLASH_PAGESIZE 2048
static uint32_t CurrentFlashAddr = APP_ADDR;

volatile uint8_t Transfer_Done_Flag = 0;

uint16_t USART1_Recvlen;
uint16_t temp_data;

void MyYmodem_SendData(uint8_t data)
{
	Serial_SendByte(data);
}



int MyYmodem_RecvData(uint8_t* pdata, int len)
{
	int i =0;
	uint32_t timeout = 50000;
	while(i<len && timeout>0)
	{
		if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
		{
			pdata[i++] = (uint8_t)USART_ReceiveData(USART1);
			timeout = 50000;
		}
		else
		{
			timeout--;
		}
	}
	return i;
}

uint32_t My_Yomodem_GetTime(void)
{
	return 0;
}

void My_Ymodem_RecvFile(char* filename, int filesize)
{
	CurrentFlashAddr = APP_ADDR;
	Serial_Printf("文件名：%s , 文件大小：%d\r\n",filename,filesize);
	uint32_t PageCount = (filesize/FLASH_PAGESIZE) + 1;
	flash_unlock();
	for(int i = 0;i<PageCount;i++)
	{
		flash_ErasePage(APP_ADDR+i*FLASH_PAGESIZE);
	}
	flash_lock();
	Serial_Printf("Flash 擦除完成\r\n");
}

void My_Ymodem_RecvFileData(uint8_t num, uint8_t* pdata, int len)
{
	uint16_t DownData;
	uint32_t pcount = 0;
	flash_unlock();
	for(uint32_t i = 0;i<len;i+=2)
	{
		DownData = *(uint16_t *)(pdata + i);
		flash_WriteHalfWord(CurrentFlashAddr,DownData);
		CurrentFlashAddr+=2;
	}
	flash_lock();
	Serial_Printf("Flash 下载完成\r\n");
}

void My_Ymodem_End(void)
{
	Serial_Printf("Ymodem 下载完成");
	Transfer_Done_Flag = 1;
	return;
}

void My_Ymodem_ErrorHandle(void)
{
	Serial_Printf("Ymodem 下载错误");
	return;
}

