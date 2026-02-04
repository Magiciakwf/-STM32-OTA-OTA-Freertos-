#ifndef __MYCAN_H_
#define __MYCAN_H_
#include "stm32f10x.h"
#include "main.h"

void MyCan_Init(void);
void MyCAN_Transmit(CanTxMsg *Tx_Msg);
uint8_t MyCAN_RecvFlag(void);
void MyCAN_Receive(CanRxMsg *Rx_Msg);

extern volatile uint8_t CAN_RecvFlag;
extern CanRxMsg RxMsg;
extern uint8_t crc_flag;

#define OTA_SEND_ID 0x701
#define OTA_RECV_ID 0x702
#define OTA_START_ID 0x700
#define OTA_END_ID 0x703

#endif