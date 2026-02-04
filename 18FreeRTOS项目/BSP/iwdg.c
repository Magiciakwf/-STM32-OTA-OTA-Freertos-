#include "iwdg.h"

void IWDG_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	while (IWDG_GetFlagStatus(IWDG_FLAG_PVU) != RESET);
	IWDG_SetPrescaler(IWDG_Prescaler_16);
	while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) != RESET);
	IWDG_SetReload(0x1388);//每隔2s就需要喂狗
	IWDG_Enable();
}