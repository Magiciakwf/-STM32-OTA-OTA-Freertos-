#ifndef __IAP_H_
#define __IAP_H_

#include "main.h"

void Jump_To_App(uint32_t AppAddr);
void DeInit_Peripherals(void);

typedef struct{
	uint8_t ota_flag;
	uint32_t data_len;
	uint32_t crc;
}OTA_Info_t;


#endif