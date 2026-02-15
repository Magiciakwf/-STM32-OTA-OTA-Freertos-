#ifndef __MYCAN_H_
#define __MYCAN_H_
#include "stm32f10x.h"

void MyCan_Init(void);
void MyCAN_Transmit(CanTxMsg *Tx_Msg);
uint8_t MyCAN_RecvFlag(void);
void MyCAN_Receive(CanRxMsg *Rx_Msg);
void Call_BusOff_Recovery_Process(void);

typedef struct{
	uint32_t EWG_Count;
	uint32_t EPV_Count;
	uint32_t BOF_Count;
	uint8_t Link_Quality;
}CAN_Health_t;

typedef enum{
	BUSOFF_NORMAL = 0,
	BUSOFF_DETECTED,
	BUSOFF_WAITING,
	BUSOFF_RECOVORY,
}BUSOFF_State_t;

extern volatile  BUSOFF_State_t g_BusOffState;
extern volatile uint32_t g_BusOffCount;

#define OTA_SEND_ID 0x701
#define OTA_RECV_ID 0x702
#define OTA_START_ID 0x700
#define OTA_END_ID 0x703
#define BUSOFF_COOLDOWN_MS 3000

#endif