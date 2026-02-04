#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include "stm32f10x.h" 

#define DMA_BUFSIZE 1024
#define RECV_SIZE 512

extern uint8_t DMA_RecvFlag;
extern uint8_t DMA_USART1Buf[1024];
extern uint8_t USART1_Recv_Buf[RECV_SIZE];
extern volatile uint8_t Rx_DoneFlag;
extern volatile uint32_t Serial_Recvcnt;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);
uint8_t Serial_GetRxData(void);
uint8_t Serial_RecvPacket(void);
uint32_t Get_PackSize(void);



#endif
