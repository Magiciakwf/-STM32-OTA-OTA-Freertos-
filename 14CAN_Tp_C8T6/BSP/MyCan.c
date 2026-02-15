#include "MyCan.h"
#include "main.h"

CAN_Health_t CAN_Health = {0};

volatile  BUSOFF_State_t g_BusOffState = BUSOFF_NORMAL;
volatile uint32_t g_BusOffCount = 0;


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
	CAN_ITConfig(CAN1, CAN_IT_EPV|CAN_IT_EWG|CAN_IT_BOF, ENABLE);//打开警告中断，被动错误中断，总线关闭中断
	
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

void Call_BusOff_Recovery_Process(void)
{
		switch(g_BusOffState)
		{
			case BUSOFF_NORMAL:
			 break;
			
			case BUSOFF_DETECTED:
			{
				g_BusOffCount = g_SystemTick_ms;
				g_BusOffState = BUSOFF_WAITING;
				break;
			}
			
			case BUSOFF_WAITING:
			{
				if(g_SystemTick_ms - g_BusOffCount > BUSOFF_COOLDOWN_MS)
				{
					g_BusOffState = BUSOFF_RECOVORY;
					break;
				}
			}
			
			case BUSOFF_RECOVORY:
			{
				g_BusOffState = BUSOFF_NORMAL;	
				CAN_DeInit(CAN1);
				MyCan_Init();
				break;
			}	
		}
}
	

// SCE中断服务函数
void CAN1_SCE_IRQHandler(void)
{
    if(CAN_GetITStatus(CAN1, CAN_IT_EWG) == SET)
    {
        CAN_Health.EWG_Count++;
        CAN_Health.Link_Quality = 1; 
        
        CAN_ClearITPendingBit(CAN1, CAN_IT_EWG);        
    }
    

    if(CAN_GetITStatus(CAN1, CAN_IT_EPV) == SET)
    {
        CAN_Health.EPV_Count++;
        CAN_Health.Link_Quality = 2; 
        

        CAN_ClearITPendingBit(CAN1, CAN_IT_EPV);        
    }
        

    if(CAN_GetITStatus(CAN1, CAN_IT_BOF) == SET)
    {
   
        CAN_Health.BOF_Count++; 
				g_BusOffState = BUSOFF_DETECTED;
/////////清除发送邮箱////////////////////////////////		
				CAN_CancelTransmit(CAN1, 0);
        CAN_CancelTransmit(CAN1, 1);
        CAN_CancelTransmit(CAN1, 2);
        CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
    }
}

