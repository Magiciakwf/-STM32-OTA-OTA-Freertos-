#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"

#define KEY1 0x45670123
#define KEY2 0xCDEF89AB

void flash_unlock(void);
void flash_lock(void);
static void flash_busy(void);
void flash_ErasePage(uint32_t addr);
void flash_WriteHalfWord(uint32_t addr,uint16_t data);

#endif