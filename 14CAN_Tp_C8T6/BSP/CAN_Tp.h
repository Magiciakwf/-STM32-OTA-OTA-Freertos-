#ifndef _CAN_TP_H_
#define _CAN_TP_H_

#include "stm32f10x.h"
#include "main.h"

void CANTP_Send(uint8_t *pdata,uint32_t payload_len,uint8_t delay_flag);
void flow_ctr(void);

#endif