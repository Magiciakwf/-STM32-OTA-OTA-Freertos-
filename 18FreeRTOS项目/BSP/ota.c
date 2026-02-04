#include "ota.h"
#include "Serial.h"
#include "ota_cfg.h"
#include "user_Ymodem.h"
#include "user_flash.h"
#include "CAN_Tp.h"
//1表示当前为APP A状态，2表示当前为APP B状态

uint8_t CRC_RxBuf[4];

uint8_t get_current_slot(void)
{
   if(SCB->VTOR == APPA_ADDR)
    {
        return 1;
    }
   if(SCB->VTOR == APPB_ADDR)
   {
        return 2;
   }
   return 0;
}

void ota_update(void)
{
    uint8_t current_slot = get_current_slot();
    uint32_t target_addr = 0;
    if(current_slot == 1)
    {
        Reset_to_CfgB();
        Serial_Printf("Now is A area,Prepare to Update B area\r\n");
    }
    else if(current_slot == 2)
    {
        Reset_Defalt_CfgA();
        Serial_Printf("Now is B area,Prepare to Update A area\r\n");
    }
    else
    {
       Serial_Printf("IT addr error\r\n") ;
    }

    if(target_addr != 0)
    {
        ymodem_start();
        while(1)
        {
            int status = ymodem_recv_process();
            if(status < 0)
            {
                Serial_Printf("Config Error\r\n");
                while(1);
            }
            if(Transfer_Done_Flag == 1)
            {
                Serial_Printf("Switch to update area\r\n");
                Transfer_Done_Flag = 0;
                NVIC_SystemReset();
            }
        }


    }
}

// 标准 CRC32 查表法 (速度快) 或者 直接计算法 (代码少)
// 这里提供直接计算法，节省 Flash 空间，逻辑简单
uint32_t OTA_Calculate_CRC32(uint32_t start_addr, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    uint32_t i, j;
    uint8_t byte;
    
    for (i = 0; i < len; i++)
    {
        // 逐字节读取 Flash 里的数据
        byte = *(__IO uint8_t*)(start_addr + i);
        
        crc ^= byte;
        for (j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return ~crc; // 取反
}

//void CRC_Compare(void)
//{
//	uint32_t expected_crc = 0;
//	uint32_t calculate_crc = 0;
//	expected_crc = (uint32_t)CRC_RxBuf[0] << 24 |
//								 (uint32_t)CRC_RxBuf[1] << 16 |
//								 (uint32_t)CRC_RxBuf[2] << 8 | 
//								 (uint32_t)CRC_RxBuf[3];
//	calculate_crc = OTA_Calculate_CRC32(target_addr,file_size);
//	if(expected_crc == calculate_crc)
//	{
//		Serial_Printf("OTA download success\r\n");
//		NVIC_SystemReset();
//	}
//	else
//	{
//		Serial_Printf("OTA download error\r\n");
//	}
//}
