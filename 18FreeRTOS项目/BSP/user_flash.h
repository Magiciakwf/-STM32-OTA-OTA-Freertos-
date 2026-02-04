#ifndef _USER_FLASH_H_
#define _USER_FLASH_H_

#define FLASH_PAGENUM 100

#include "main.h"

void user_erase_start(void);
void user_flash_write(uint32_t len,uint8_t *pBuf);

extern uint32_t target_addr;


#endif