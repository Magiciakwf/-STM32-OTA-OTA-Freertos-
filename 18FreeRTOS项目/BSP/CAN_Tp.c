#include "CAN_Tp.h"
#include "MyCAN.h"
#include "user_flash.h"
#include "ota.h"
#include "queue.h"
#include "start_task.h"
#include <string.h>

static uint8_t CANTP_RecvBuf[CANTP_RECVSIZE];
static uint8_t Expected_SN = 0;
static uint8_t *pCANTP_RecvBuf;
uint8_t status = IDLE;
uint32_t Recv_len = 0;
uint32_t copy_len = 0;

OTA_Msg_t OTA_Msg;

uint8_t CAN_TP_Recv(void)
{
	
	uint8_t temp = 0;
	uint8_t erase_flag = 0;
	BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
	
	if(RxMsg.Data[0] == 0xEE && RxMsg.StdId == OTA_START_ID)//处理起始包
	{
		OTA_Msg.type = START;//塞进队列里的类型为START
		
		OTA_Msg.data.file_size = RxMsg.Data[1] << 24 |
								RxMsg.Data[2] << 16 |
								RxMsg.Data[3] << 8  |
								RxMsg.Data[4];
		xQueueSendFromISR(OTA_Queue, &OTA_Msg, &pxHigherPriorityTaskWoken);//这是中断中调用的函数，不能用普通的Send函数
	}
	
	if(RxMsg.Data[0] == 0xFF && RxMsg.StdId == OTA_END_ID)
	{
		OTA_Msg.type = END;//塞进队列里的类型为END
		for (int i = 0;i<4;i++)
		{
			OTA_Msg.data.crc_buf[i] = RxMsg.Data[i+1];
			xQueueSendFromISR(OTA_Queue, &OTA_Msg, &pxHigherPriorityTaskWoken);
		}
	}
	
	temp = (RxMsg.Data[0] >> 4) & 0x0F;//提取PCI
	if(temp == 0x01)
	{
		copy_len = 0;//如果数据传输失败，copy_len不清0会导致copy_len叠加

			
		OTA_Msg.Recv_len = ((RxMsg.Data[0] & 0x0F) << 8)|RxMsg.Data[1];//<<优先级高于&
		Recv_len = OTA_Msg.Recv_len;//因为后面都用到了Recv_len
		
		pCANTP_RecvBuf = CANTP_RecvBuf;
		for (int i = 0;i<6;i++)
		{
			CANTP_RecvBuf[i] = RxMsg.Data[i+2];
			pCANTP_RecvBuf++;
			copy_len++;
		}
		status = BUSY;
		Expected_SN = 1;
		return 0;//成功处理
	}
	
	if(temp == 0x02)
	{
		OTA_Msg.type = DATA;//塞进队列里为DATA类型
		if(status!=BUSY)return 1;//
		uint8_t current_sn = 0;
		current_sn = RxMsg.Data[0] & 0x0F;//获取SN
		if(current_sn == Expected_SN)
		{
			Expected_SN++;
			Expected_SN%=16;
			for (int i=0;i<7;i++)
			{
				if(copy_len < Recv_len)
				{
					*pCANTP_RecvBuf++ = RxMsg.Data[i+1];
					copy_len++;
				}
				if(copy_len >= Recv_len)
				{
					memcpy(OTA_Msg.data.CANTP_RecvBuf,CANTP_RecvBuf,CANTP_RECVSIZE);
					xQueueSendFromISR(OTA_Queue, &OTA_Msg, &pxHigherPriorityTaskWoken);//把数据塞进队列里
					status = IDLE;		
					copy_len = 0;		
					return 0;
				}
			}
		}
		else
		{
			status = IDLE;
			return 2;//错误：SN不匹配
		}
	}
}