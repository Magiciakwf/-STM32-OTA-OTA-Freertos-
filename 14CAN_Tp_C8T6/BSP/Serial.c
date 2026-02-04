#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Serial.h"


//uint8_t DMA_USART1Buf[DMA_BUFSIZE];
//uint8_t DMA_RecvFlag = 0;
uint8_t Serial_RxData;
uint8_t USART1_Recv_Buf[RECV_SIZE];
volatile uint32_t Serial_Recvcnt = 0;
volatile uint8_t Rx_DoneFlag = 0;


void Serial_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//////////////////注释掉DMA和中断以适配Ymodem///////////////////////
	
//	DMA_InitTypeDef DMA1_InitStructure;
//	DMA1_InitStructure.DMA_BufferSize = DMA_BUFSIZE;
//	DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设到内存
//	DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA1_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_USART1Buf;
//	DMA1_InitStructure.DMA_MemoryDataSize = 8;
//	DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA1_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA1_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
//	DMA1_InitStructure.DMA_PeripheralDataSize = 8;
//	DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA1_InitStructure.DMA_Priority = DMA_Priority_Medium;
//	DMA_Init(DMA1_Channel5, &DMA1_InitStructure);
//	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
///////////////////////////////////////////////////////////////////	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);//先使能串口
	volatile uint32_t temp = USART1->SR;//清除标志位
  temp = USART1->DR;
	
//	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
//////////////需要同时开接收中断和空闲中断//////////////
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
/////////只有当发完数据后，空闲状态才会触发中断
//	DMA_Cmd(DMA1_Channel5,ENABLE);
	

}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxData(void)
{
	Serial_RxData = USART_ReceiveData(USART1);
	return Serial_RxData;			//返回接收的数据变量
}

//uint8_t Serial_RecvPacket(void)
//{
//	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
//	{
//			USART1_Recv_Buf[Serial_Cnt] = Serial_GetRxData();
//			Serial_Cnt++;
//	}

//}

uint32_t Get_PackSize(void)
{
	return Serial_Recvcnt;
}

///////////////注释掉空闲中断服务函数以适配Ymodem////////////////////
void USART1_IRQHandler(void)
{
	volatile uint32_t temp;
	//数据接收部分
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
	{
		if(Serial_Recvcnt<RECV_SIZE)
		{
			USART1_Recv_Buf[Serial_Recvcnt++] = Serial_GetRxData();
		}
		else
		{
			Serial_GetRxData();//只有读了数据才能清除标志位，不然就会一直跳进中断里
		}
	}
	//接收完成后置高标志位
	if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE) == SET)
	{
		temp = USART1->SR;
		temp = USART1->DR;
		Rx_DoneFlag = 1;
		Serial_Recvcnt = 0;
	}
}
