#ifndef __MYCAN_H_
#define __MYCAN_H_
#include "stm32f10x.h"

void MyCan_Init(void);
void MyCAN_Transmit(CanTxMsg *Tx_Msg);
uint8_t MyCAN_RecvFlag(void);
void MyCAN_Receive(CanRxMsg *Rx_Msg);

#define OTA_SEND_ID 0x701
#define OTA_RECV_ID 0x702
#define OTA_START_ID 0x700
#define OTA_END_ID 0x703

#endif