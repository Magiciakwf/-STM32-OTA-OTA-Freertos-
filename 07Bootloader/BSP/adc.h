#ifndef __ADC_h_
#define __ADC_h_

#include "main.h"
#define Buf_Size 128
extern uint16_t DMA_Buf[Buf_Size];
void UserADC_Init(void);
#endif