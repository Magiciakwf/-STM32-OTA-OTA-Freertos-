#ifndef _OTA_CFG_H_
#define _OTA_CFG_H_
#include "stm32f10x.h"

#define APPB_ADDR 0x8037000//APP AºÍB¸÷200K
#define PARM_ADDR 0x807FC00
#define APPA_ADDR 0x8005000

#define MAGIC_NUM 0xFFCD

typedef enum{
	BOOT_LOAD_APP_A = 0xAAAA,
	BOOT_LOAD_APP_B = 0xBBBB,
}BootTarget_t;

typedef enum{
	OTA_STATUS_TEXTING = 1,
	OTA_STATUS_NORMAL = 0,
}OTA_Status_t;

typedef struct{
	uint32_t magic_num;
	uint32_t ota_state;
	uint32_t retry_cnt;
	uint32_t boot_target;
}SystemConfig_t;

void APP_Jump(uint32_t addr);
void Reset_Defalt_CfgA(void);
void Reset_to_CfgB(void);
void Ota_State_Set(uint8_t state);
void Ota_Retrycnt_Set(uint16_t cnt);
	
#endif