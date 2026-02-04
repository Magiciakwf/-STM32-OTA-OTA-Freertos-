#include "MyCan.h"




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
	CAN_Instruct.CAN_SJW = CAN_SJW_2tq;
	CAN_Instruct.CAN_ABOM = DISABLE;
	CAN_Instruct.CAN_AWUM = ENABLE;
	CAN_Instruct.CAN_NART = DISABLE;
	CAN_Instruct.CAN_RFLM = ENABLE;
	CAN_Instruct.CAN_TTCM = DISABLE;
	CAN_Instruct.CAN_TXFP = DISABLE;
	CAN_Init(CAN1, &CAN_Instruct);
	
	CAN_Filter_Instruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_Filter_Instruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_Filter_Instruct.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_Filter_Instruct.CAN_FilterIdHigh = 0x0000;
	CAN_Filter_Instruct.CAN_FilterIdLow = 0x0000;//过滤ID号为0x345标准ID的遥控帧
	CAN_Filter_Instruct.CAN_FilterMaskIdHigh =0x0000;
	CAN_Filter_Instruct.CAN_FilterMaskIdLow = 0x0000;
	CAN_Filter_Instruct.CAN_FilterNumber = 0;
	CAN_Filter_Instruct.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_Filter_Instruct);
	
//	CAN_Filter_Instruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
//	CAN_Filter_Instruct.CAN_FilterMode = CAN_FilterMode_IdMask;
//	CAN_Filter_Instruct.CAN_FilterScale = CAN_FilterScale_16bit;
//	CAN_Filter_Instruct.CAN_FilterIdHigh = 0x400<<5;
//	CAN_Filter_Instruct.CAN_FilterIdLow = 0x789<<5|0x10;//过滤ID号为0x345标准ID的遥控帧
//	CAN_Filter_Instruct.CAN_FilterMaskIdHigh = 0x890<<5;
//	CAN_Filter_Instruct.CAN_FilterMaskIdLow = 0x1111;
//	CAN_Filter_Instruct.CAN_FilterNumber = 1;
//	CAN_Filter_Instruct.CAN_FilterActivation = ENABLE;	
//	CAN_FilterInit(&CAN_Filter_Instruct);
}

void MyCAN_Transmit(CanTxMsg *Tx_Msg)
{
	
	uint8_t Trans_MailBox = CAN_Transmit(CAN1, Tx_Msg);
	while(CAN_TransmitStatus(CAN1, Trans_MailBox)!=CAN_TxStatus_Ok);
}

uint8_t MyCAN_RecvFlag(void)
{
	if(CAN_MessagePending(CAN1, CAN_Filter_FIFO0)!=0)
	{
		return 1;
	}
	return 0;
}

void MyCAN_Receive(CanRxMsg *Rx_Msg)
{

	if(CAN_MessagePending(CAN1, CAN_Filter_FIFO0)>1)
	{
		CAN_Receive(CAN1, CAN_Filter_FIFO0, Rx_Msg);
	}
}


