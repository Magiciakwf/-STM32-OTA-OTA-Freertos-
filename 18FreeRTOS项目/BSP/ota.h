#ifndef _OTA_H_
#define _OTA_H_

#include "stm32f10x.h"

void ota_update(void);
uint8_t get_current_slot(void);
uint32_t OTA_Calculate_CRC32(uint32_t start_addr, uint32_t len);
void CRC_Compare(void);

extern uint8_t CRC_RxBuf[4];

#endif