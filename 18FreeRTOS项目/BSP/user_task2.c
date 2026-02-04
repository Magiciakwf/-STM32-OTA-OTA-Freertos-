#include "user_task2.h"
#include "Serial.h"
#include "start_task.h"
#include "user_flash.h"
#include "queue.h"
#include "ota.h"
#include "user_flash.h"
#include "CAN_Tp.h"

static uint8_t cmd;

Job_t Job_Recv;

void Flash_Write_Task(void *pvParameters)
{

	while(1)
	{
		xQueueReceive(Job_Queue,&Job_Recv,portMAX_DELAY);
		if(Job_Recv.cmd == ERASE)
		{
			user_erase_start();
		}
		
		if(Job_Recv.cmd == WRITE)
		{
			user_flash_write(Job_Recv.Recv_len,Job_Recv.write_buf);
		}
	}

	
}