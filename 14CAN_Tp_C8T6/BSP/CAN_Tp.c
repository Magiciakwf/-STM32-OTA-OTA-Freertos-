#include "CAN_Tp.h"
#include "Serial.h"
#include "delay.h"
#include "MyCAN.h"
#include <string.h>

#define FRAME_HEADER  0xA5
#define CMD_START     0x01
#define CMD_DATA      0x02
#define CMD_END       0x03

uint8_t Pci = 0;
uint8_t SN = 0;
uint8_t CANTP_SendBuf[512];
uint8_t *pCANTP_SendBuf;
uint32_t total_size;
uint32_t CANTP_PacketSize = 0;


CanTxMsg TxMsg = {
	 .StdId = OTA_SEND_ID,
	 .ExtId = 0x00000000,
	 .IDE = CAN_Id_Standard,
	 .RTR = CAN_RTR_Data,
	 .DLC = 8,//ISO-TP协议中DLC必须为8
};


void CANTP_Send(uint8_t *pdata,uint32_t payload_len,uint8_t delay_flag)//无法接收小于6字节的数据
{
	static uint8_t FF_Flag = 0;
	uint8_t temp_buf[8];
	uint32_t copy_len = 0;//已发有效数据字节数
	uint32_t send_size = 0 ;
//	total_size = Get_PackSize();	//有效数据长度
	CANTP_PacketSize = ((payload_len-6)/7)*8+((payload_len-6)%7+1)+8;//加工后的数据长度
	uint32_t Loop_cnt = CANTP_PacketSize/8+1;
	// 【新增】判断是否为单帧 (<= 7 字节)
    if (payload_len <= 7) 
    {
        memset(temp_buf, 0x00, 8); // 填充0，符合某些车企Padding规范
        // 单帧头：0x00 | 长度
        temp_buf[0] = (uint8_t)payload_len; 
        
        for(int i = 0; i < payload_len; i++) {
            temp_buf[i+1] = *pdata++;
        }
        
        memcpy(TxMsg.Data, temp_buf, 8);
        MyCAN_Transmit(&TxMsg);
        return; // 发送完直接结束
    }
////////////////FC//////////////////////////////	
	Pci = 0x1;
	temp_buf[0] = (Pci<<4)|(payload_len>>8&0xf);
	temp_buf[1] = payload_len&0xff;
	for (int i = 0;i<6;i++)
	{
		temp_buf[i+2] = *pdata++;
	}
	copy_len+=6;
	memcpy(TxMsg.Data,temp_buf,8);//第一个和第二个参数均为地址
	MyCAN_Transmit(&TxMsg);
	Delay_ms(2);
	
	
	SN = 1;//更新 SN: 1 -> 2 ... -> 15 -> 0 -> 1
///////////////FC////////////////////////////////
	
	while(copy_len<payload_len)
	{
		memset(temp_buf,0x00,8);//第一个参数是地址，第二个是值
		Pci = 0x2;
		temp_buf[0] = (Pci<<4)|(SN&0xF);
		for (int i = 0;i<7;i++)
		{
			if(copy_len<payload_len)
			{
				temp_buf[i+1] = *pdata++;
				copy_len++;
			}
		}
		memcpy(TxMsg.Data,temp_buf,8);
		MyCAN_Transmit(&TxMsg);
		SN++;
    if (SN > 15) SN = 0; // ISO-TP 规定 SN 到 15 后回绕到 0
		Delay_ms(2);	
  }
}

void flow_ctr(void)
{
	if(USART1_Recv_Buf[0]!=0xA5)
	{
		Serial_Printf("数据首帧错误\r\n");
		goto _exit;
	}
	uint8_t FF_Flag = 0;
	uint8_t cmd = USART1_Recv_Buf[1];
	uint32_t payload_len = (uint16_t)USART1_Recv_Buf[2]<<8|USART1_Recv_Buf[3];
	uint8_t *pdata = &USART1_Recv_Buf[4];
	switch (cmd)
	{
		case CMD_START://首包和尾包都有八个四节
		{
			Serial_Printf("数据首帧开始发送\r\n");
			CanTxMsg First_TxMsg;
      First_TxMsg.StdId = OTA_START_ID; 
      First_TxMsg.DLC = 5;
			First_TxMsg.RTR = CAN_RTR_Data;
      First_TxMsg.Data[0] = 0xEE; // 自定义擦除指令
			First_TxMsg.IDE = CAN_Id_Standard;
			for (int i = 0;i<4;i++)
			{
				First_TxMsg.Data[i+1] = USART1_Recv_Buf[i+4];//发送file_size
			}
			
			FF_Flag = 1;
      MyCAN_Transmit(&First_TxMsg);
			break;
		}
		case CMD_DATA:
		{
			printf("CMD: Data Transmit Len:%d\r\n", payload_len);
			CANTP_Send(pdata, payload_len,FF_Flag);
			FF_Flag = 0;
			break;
		}
		case CMD_END:
		{
			CanTxMsg End_TxMsg;
			End_TxMsg.DLC = 5;
			End_TxMsg.IDE = CAN_Id_Standard;
			End_TxMsg.RTR = CAN_RTR_Data;
			End_TxMsg.StdId = OTA_END_ID;
			End_TxMsg.Data[0] = 0xFF;
			for (int i = 0;i<4;i++)
			{
				End_TxMsg.Data[i+1] = USART1_Recv_Buf[i+4];
			}
			 MyCAN_Transmit(&End_TxMsg);
			printf("CMD: End OTA\r\n");
			break;
		}
	}
	
	_exit:
	{
		Rx_DoneFlag = 0;
	}
	
}


/////原先思路是定义一个数据接收完整个串口缓存数组的数据然后再发出去，这样的话会非常消耗RAM资源
//void CANTP_Send(void)
//{
//	uint32_t Loop_Cnt = 0;
//	pCANTP_SendBuf = USART1_Recv_Buf;
//	if(pCANTP_SendBuf!=NULL)
//	{ 	
//		total_size = Get_PackSize();
//		CANTP_PacketSize = 8+((total_size-6)/7)*8+total_size-8-((total_size-6)/7)*8;
/////////////////FF帧的处理////////////////////////////////		
//		Pci = 0x1;
//		CANTP_SendBuf[0] = (Pci<<4)|(total_size>>8&0xf);
//		CANTP_SendBuf[1] = total_size&0xff;
//		Loop_Cnt = (total_size/7+1)*8;
//		for (int i = 0;i<6;i++)
//		{
//			CANTP_SendBuf[i+2] = *pCANTP_SendBuf;
//			pCANTP_SendBuf++;
//		}
/////////////////FC帧的处理////////////////////////////////
//		Pci = 0x2;
//		for (int i = 0;i<Loop_Cnt;i++)
//		{
//			if(i % 8 == 0)
//			{
//				CANTP_SendBuf[i+8] = (Pci<<4)|(SN&0xf);
//				SN++;
//				SN %= 16;
//			}
//			else
//			{
//				if(pCANTP_SendBuf!=NULL)
//				{
//					CANTP_SendBuf[i+8] = *pCANTP_SendBuf;
//					pCANTP_SendBuf++;
//				}
//			}	
//		}
//		DoneFlag = 1;

//	  }
//////////////////////数据的发送/////////////////////////////	
//		if(DoneFlag)
//		{
//			DoneFlag = 0;
//			for (int i = 0;i<CANTP_PacketSize;i++)
//			{
//				TxMsg.Data[i%8] = CANTP_SendBuf[i];
//				if(i%8 == 7)
//				{
//					MyCAN_Transmit(&TxMsg);
//					Delay_ms(2);
//				}
//			}
//		}
//}