#include "flash.h"

void flash_unlock(void)
{
	
	if((FLASH->CR & 1<<7) != RESET)
	{
		__disable_irq();
		FLASH->KEYR = KEY1;
		FLASH->KEYR = KEY2;
		__enable_irq();
	}
}

void flash_lock(void)
{
	FLASH->CR |= (1<<7);
}

static void flash_busy(void)
{
	while((FLASH->SR & 0x1) == 1)
	{
		
	}
}

void flash_ErasePage(uint32_t addr)
{
	flash_busy();
	FLASH->CR |= (1<<1);
	FLASH->AR = addr;
	FLASH->CR |= (1<<6);
	flash_busy();
	FLASH->CR &= ~(1<<1);
}

void flash_WriteHalfWord(uint32_t addr,uint16_t data)
{
	
	flash_busy();
	FLASH->CR |= 1;
	*(__IO uint16_t *)addr = data;
	flash_busy();
	FLASH->CR &= ~1;
}

