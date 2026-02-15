#include "stm32f10x.h"

void delay(uint16_t time);

extern CanTxMsg TxMsg;
extern volatile uint32_t g_SystemTick_ms;
