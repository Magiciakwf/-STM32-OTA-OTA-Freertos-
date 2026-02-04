#include "MyCan.h"
#include "CAN_Tp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "semphr.h"
#include "queue.h"
#include "start_task.h"

volatile uint8_t CAN_RecvFlag = 0;//在中断里使用得用volatile
CanRxMsg RxMsg;
OTA_Msg_t cmd_Msg;



void MyCan_Init(void)
{
	GPIO_InitTypeDef GPIO_Instruct;
	CAN_InitTypeDef CAN_Instruct;
	CAN_FilterInitTypeDef CAN_Filter_Instruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	
	GPIO_Instruct.GPIO_Mode = GPIO_Mode_AF_PP;//必须配置成复用推挽模式，不然会发不出去
	GPIO_Instruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_Instruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Instruct);
	
	GPIO_Instruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Instruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_Instruct);
	
	
	CAN_Instruct.CAN_Mode = CAN_Mode_Normal;		
	CAN_Instruct.CAN_Prescaler = 6;
	CAN_Instruct.CAN_BS1 = CAN_BS1_8tq;//不能直接填数字8
	CAN_Instruct.CAN_BS2 = CAN_BS1_8tq;
	CAN_Instruct.CAN_SJW = CAN_SJW_2tq;//波特率问题，不是标准的250,500,1000
	CAN_Instruct.CAN_ABOM = DISABLE;
	CAN_Instruct.CAN_AWUM = ENABLE;
	CAN_Instruct.CAN_NART = DISABLE;
	CAN_Instruct.CAN_RFLM = ENABLE;
	CAN_Instruct.CAN_TTCM = DISABLE;
	CAN_Instruct.CAN_TXFP = DISABLE;
	CAN_Init(CAN1, &CAN_Instruct);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//Group配置问题
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	
	CAN_Filter_Instruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_Filter_Instruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_Filter_Instruct.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_Filter_Instruct.CAN_FilterIdHigh = 0x0000;
	CAN_Filter_Instruct.CAN_FilterIdLow = 0x0000;//过滤ID号为0x345标准ID的遥控帧
	CAN_Filter_Instruct.CAN_FilterMaskIdHigh = 0x0000;
	CAN_Filter_Instruct.CAN_FilterMaskIdLow = 0x0000;
	CAN_Filter_Instruct.CAN_FilterNumber = 0;
	CAN_Filter_Instruct.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_Filter_Instruct);
	
}

void MyCAN_Transmit(CanTxMsg *Tx_Msg)
{
	
	uint8_t Trans_MailBox = CAN_Transmit(CAN1, Tx_Msg);
	while(CAN_TransmitStatus(CAN1, Trans_MailBox)!=CAN_TxStatus_Ok);
}

uint8_t MyCAN_RecvFlag(void)
{
	if(CAN_MessagePending(CAN1, CAN_Filter_FIFO0)>1)
	{
		return 1;
	}
	return 0;
}

void MyCAN_Receive(CanRxMsg *Rx_Msg)
{

	if(CAN_MessagePending(CAN1, CAN_Filter_FIFO0)!=0)//判断条件为！=0而不是>1
	{
		CAN_Receive(CAN1, CAN_Filter_FIFO0, Rx_Msg);
	}
}

///////////////////CAN接收中断函数///////////////////
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		MyCAN_Receive(&RxMsg);//读数据清除中断标志位
//		if(RxMsg.StdId == OTA_SEND_ID || RxMsg.StdId == OTA_END_ID)
//		{
//			cmd_Msg.cmd = RxMsg.Data[0];
//			if(OTA_Queue != NULL)
//			{
//				xQueueSendFromISR(OTA_Queue, &cmd_Msg, &xHigherPriorityTaskWoken);//标准的队列发送是阻塞态，所以在中断中只能使用特定的队列发送函数
////这个Quene的数据是否唤醒了更高优先级的任务，是的话就xHigherPriorityTaskWoken切换成pdTRUE
////				xQueueSend(cmd_Queue, (void *)&cmd, pdMS_TO_TICKS(0));
//			}
//		}
//		
		if(RxMsg.StdId == OTA_SEND_ID || RxMsg.StdId == OTA_START_ID || RxMsg.StdId == OTA_END_ID)//判断条件写错了，中间那个写成了SEND_ID
		{
			CAN_TP_Recv();
		}

	}
}


