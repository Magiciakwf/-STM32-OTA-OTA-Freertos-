#ifndef _USERYMODEM_H_
#define _USERYMODEM_H_

#include "ymodem.h"
#include "stm32f10x.h" 
#include "Serial.h"

#define APP_ADDR 0x8008000

void MyYmodem_SendData(uint8_t data);
int MyYmodem_RecvData(uint8_t* pdata, int len);
void My_Ymodem_RecvFile(char* filename, int filesize);
void My_Ymodem_RecvFileData(uint8_t num, uint8_t* pdata, int len);
void My_Ymodem_End(void);
void My_Ymodem_ErrorHandle(int err);
uint32_t My_Yomodem_GetTime(void);

extern volatile uint8_t Transfer_Done_Flag;

#endif