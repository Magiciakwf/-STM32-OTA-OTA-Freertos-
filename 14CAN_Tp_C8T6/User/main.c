#include "stm32f10x.h"
#include "main.h"
#include "MyCAN.h"
#include "Serial.h"
#include "key.h"
#include "delay.h"
#include "CAN_Tp.h"

volatile uint32_t g_SystemTick_ms = 0;//计数systick,在systick_handle使用

int main()
{
	uint8_t pTxMsgArray = 0;	
	uint8_t USART1_Recv_Buf[256];
	uint8_t USART1_Recv_Flag = 0;

//	uint8_t serial_cnt = 0;
//	uint8_t Pci = 0;
//	uint8_t SN = 0;
//	uint8_t CANTP_SendBuf[256];
//	uint8_t *pCANTP_SendBuf;
//	uint8_t USART_RecvDoneFlag = 0;
//	int total_len = 0;
	
	


	CanRxMsg RxMsg;
	
	Serial_Init();
	MyCan_Init();
	KEY_Init();

  while(1)
	{
		if(Rx_DoneFlag == 1)
		{
			flow_ctr();
		}
		
		Call_BusOff_Recovery_Process();
		

		
		
		
//////////////////////接收八字节数据/////////////////////////////		
//	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
//    {
//        uint32_t temp_len = 0;
//        USART1_Recv_Buf[serial_cnt] = Serial_GetRxData(); 	
//        serial_cnt++; 
////////////////判断FF帧并接收///////////////////////////////			
//				if(serial_cnt == 8)
//				{
//					Pci = USART1_Recv_Buf[0]>>4;
//					if(Pci == 0x1)
//					{
//						total_len = ((uint32_t)USART1_Recv_Buf[0]&3)<<8|USART1_Recv_Buf[1];
//						temp_len = total_len;
//						for (int i = 0;i<6;i++)
//						{
//							CANTP_SendBuf[i] = USART1_Recv_Buf[i+2];
//						}
//						total_len-=6;
//					}
//				}
//				
//				if(serial_cnt>8)
//				{
//					uint32_t loop_cnt = 0;
//					Pci = USART1_Recv_Buf[8]>>4;
//					loop_cnt = total_len/7+1;//循环次数
//					if(Pci == 0x2)
//					{
//						pCANTP_SendBuf = &USART1_Recv_Buf[9];
//						for(uint32_t i = 0;i<loop_cnt;i++)
//						{
//							for(int i = 0;i<7;i++)
//							{
//								if(pCANTP_SendBuf!=NULL)
//								{
//									CANTP_SendBuf[temp_len - total_len + i] = *pCANTP_SendBuf;
//									if(i!=6)
//										pCANTP_SendBuf++;
//									else
//										pCANTP_SendBuf+=2;
//									total_len-=7;
//								}
//							}
//								
//						}
//						USART_RecvDoneFlag = 1;
//					}
//				}
//				
//				
//        if (total_len<=0 && USART_RecvDoneFlag)
//        {  
//						pCANTP_SendBuf = CANTP_SendBuf;
//            for (int i = 0; i < temp_len; i++)
//            {
//								if(pCANTP_SendBuf!=NULL)
//								{
//									TxMsg.Data[0] = USART1_Recv_Buf[i];
//									MyCAN_Transmit(&TxMsg);
//									pCANTP_SendBuf++;
//									Delay_ms(2);
//								}
//            }     
//            serial_cnt = 0; 
//        }
//    }
///////////////////////////////////////////////////////////////////
		
		
		if(MyCAN_RecvFlag() == 1)
		{
			MyCAN_Receive(&RxMsg);
		}
		Serial_Printf("id:%x\r\ndata[0]:%x,date[1]:%x,data[2]:%x,data[3]:%x\r\nID_type:%x\r\nData_Type:%x\r\n",RxMsg.StdId,RxMsg.Data[0],RxMsg.Data[1],RxMsg.Data[2],RxMsg.Data[3],RxMsg.IDE,RxMsg.RTR);

	}
		 
   
}


