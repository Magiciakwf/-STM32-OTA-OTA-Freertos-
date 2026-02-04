#include "ota_cfg.h"
#include "flash.h"


typedef void(*pFunction)(void);
pFunction JumpToAPP;

void APP_Jump(uint32_t addr)
{
	if(((*(__IO uint32_t *)addr)&0x2FFE0000) == 0x20000000)
	{
		uint32_t JumpAddr = *(__IO uint32_t *)(addr+4);
		JumpToAPP = (pFunction)JumpAddr;//实质上执行的都是解引用的地址
		__disable_irq();
		__set_MSP(*(__IO uint32_t *)addr);
		JumpToAPP();
	}
}

void Reset_Defalt_CfgA(void)
{
	flash_unlock();
	flash_ErasePage(PARM_ADDR);
	flash_WriteHalfWord(PARM_ADDR,0xFFCD);
	flash_WriteHalfWord(PARM_ADDR+2,0x0000);
	flash_WriteHalfWord(PARM_ADDR+4,OTA_STATUS_TEXTING);
	flash_WriteHalfWord(PARM_ADDR+6,0x0000);
	flash_WriteHalfWord(PARM_ADDR+8,0x0007);//retry_cnt次数为3，二进制为0111
	flash_WriteHalfWord(PARM_ADDR+10,0x0000);
	flash_WriteHalfWord(PARM_ADDR+12,0xAAAA);
	flash_WriteHalfWord(PARM_ADDR+14,0x0000);
	flash_lock();
}

void Reset_to_CfgB(void)
{
	flash_unlock();
	flash_ErasePage(PARM_ADDR);
	flash_WriteHalfWord(PARM_ADDR,0xFFCD);
	flash_WriteHalfWord(PARM_ADDR+2,0x0000);
	flash_WriteHalfWord(PARM_ADDR+4,OTA_STATUS_TEXTING);
	flash_WriteHalfWord(PARM_ADDR+6,0x0000);
	flash_WriteHalfWord(PARM_ADDR+8,0x0007);//retry_cnt次数为3,二进制为0111
	flash_WriteHalfWord(PARM_ADDR+10,0x0000);
	flash_WriteHalfWord(PARM_ADDR+12,0xBBBB);
	flash_WriteHalfWord(PARM_ADDR+14,0x0000);
	flash_lock();
}
//遇到的问题：由于flash只能把1变成0，无法把0变成1，就导致这个State_Set失败了（0写1的操作）
void Ota_State_Set(uint8_t state)
{
	flash_unlock();
	flash_WriteHalfWord(PARM_ADDR+4,state);
	flash_lock();
}

void Ota_Retrycnt_Set(uint16_t cnt)
{
	flash_unlock();
	flash_WriteHalfWord(PARM_ADDR+8,cnt);
	flash_lock();
}
	