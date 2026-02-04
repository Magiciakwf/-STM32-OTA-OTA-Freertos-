#include "user_task1.h"
#include "Serial.h"
#include "CAN_Tp.h"
#include "start_task.h"
#include "user_flash.h"
#include "ota.h"
#include <string.h>


OTA_Msg_t OTA_MsgCmd;
Job_t Job_Send;
uint32_t file_size;


void Protocol_Task(void *pvParameters)
{
	uint8_t cmd = 0;
	UBaseType_t freeStackWords;
	while(1)
	{
		xQueueReceive(OTA_Queue,&OTA_MsgCmd,portMAX_DELAY);//如果没数据的话就阻塞
		switch (OTA_MsgCmd.type)
		{
			case START:
			{
				file_size = OTA_MsgCmd.data.file_size;
				Job_Send.cmd = ERASE;
				xQueueSend(Job_Queue,&Job_Send,portMAX_DELAY);
				break;
			}
			case DATA:
			{
				Job_Send.cmd = WRITE;
				Job_Send.Recv_len = OTA_MsgCmd.Recv_len;
				memcpy(Job_Send.write_buf,OTA_MsgCmd.data.CANTP_RecvBuf,CANTP_RECVSIZE);
				xQueueSend(Job_Queue,&Job_Send,portMAX_DELAY);
				break;
			}
			case END:
			{
					uint32_t expected_crc = 0;
					uint32_t calculate_crc = 0;
					expected_crc = (uint32_t)OTA_MsgCmd.data.crc_buf[0] << 24 |
												 (uint32_t)OTA_MsgCmd.data.crc_buf[1] << 16 |
												 (uint32_t)OTA_MsgCmd.data.crc_buf[2] << 8 | 
												 (uint32_t)OTA_MsgCmd.data.crc_buf[3];
					calculate_crc = OTA_Calculate_CRC32(target_addr,file_size);
					if(expected_crc == calculate_crc)
					{
						Serial_Printf("OTA download success\r\n");//
						NVIC_SystemReset();
					}
					else
					{
						Serial_Printf("OTA download error\r\n");//
					}
					break;
			}
		}
		freeStackWords = uxTaskGetStackHighWaterMark(NULL);

    // 2. 打印（建议转换成字节，方便阅读）
        // 假设是 STM32，乘以 4 才是字节数
    Serial_Printf("Task Min Free Stack: %ld Words (%ld Bytes)\r\n", 
    freeStackWords, 
    freeStackWords * 4);
	}	
}