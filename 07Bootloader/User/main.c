#include "stm32f10x.h"
#include "main.h"
#include "adc.h"
#include "Serial.h"
#include "flash.h"
#include "iap.h"
#include "key.h"
#include "ymodem.h"
#include "user_Ymodem.h"
#include "ota_cfg.h"

#define FLASH_ADDR 0x807FC00




int  main()
{
	volatile uint8_t temp;
///////////////Ymodem相关函数///////////////////////////
//	putc_func_t Ymodem_Send;
//	putc_func_t Ymodem_Read;
//	ymodem_file_handler_t Ymodem_RecFileCallback;
//	ymodem_data_handler_t Ymodem_RecFileDataCallback;
//	ymodem_end_handler_t Ymodem_EndCallback;
//	ymodem_error_handler_t Ymodem_ErrorCallback;
//	ym_uptime_get_t Ymodem_TimCallback;
/////////////////////////////////////////////////////	
//	uint16_t USART1_Recvlen;
//	uint16_t temp_data;
	
	SystemConfig_t *pConfig = (SystemConfig_t *)PARM_ADDR;
	KEY_Init();
	Serial_Init();
//	IWDG_Init();
	ymodem_init(MyYmodem_SendData,MyYmodem_RecvData,My_Ymodem_RecvFile,
	My_Ymodem_RecvFileData,My_Ymodem_End,My_Ymodem_ErrorHandle,My_Yomodem_GetTime);
	Serial_Printf("Bootloader success\r\n");
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
	{
		Serial_Printf("由看门狗引起的复位\r\n");
	}
	
/////////////////////Bootloader跳转程序///////////////////////	
	
	

/////////////////////////////////////////////////////////////////////	
  while(1)
	{
//////////////////////////////////////////////////////////////////////////////////////		
		if(pConfig->magic_num != 0xFFCD)
		{
			Reset_Defalt_CfgA();
			NVIC_SystemReset();
		}
			temp = pConfig->add_retry_cnt;
			Serial_Printf("temp : %d",temp);
			APP_Jump(APPB_ADDR);
	
///////////////////////////////////////////////////////////AB分区跳转程序/////////////////////////////////////////////		
		//被编译器优化了，因为后面没用到temp
		
		if(pConfig->boot_target == BOOT_LOAD_APP_B && pConfig->retry_cnt!=0 && pConfig->ota_state == OTA_STATUS_TEXTING)
		{
			Serial_Printf("Retry_cnt>0,跳转至B区\r\n");
			Ota_Retrycnt_Set(0);
			APP_Jump(APPB_ADDR);
		}
		if(pConfig->boot_target == BOOT_LOAD_APP_B&& pConfig->retry_cnt==0 && pConfig->ota_state == OTA_STATUS_TEXTING)
		{
			Serial_Printf("B区程序错误,跳转至A区\r\n");
			APP_Jump(APPA_ADDR);
		}
		if(pConfig->boot_target == BOOT_LOAD_APP_A && temp!=0 && pConfig->ota_state == OTA_STATUS_TEXTING)
		{
			Serial_Printf("Retry_cnt>0,跳转至A区\r\n");
			Ota_Retrycnt_Set(0);
			APP_Jump(APPA_ADDR);
		}
		if(pConfig->boot_target == BOOT_LOAD_APP_A&& temp==0 && pConfig->ota_state == OTA_STATUS_TEXTING)
		{
			Serial_Printf("A区程序错误,跳转至B区\r\n");
			APP_Jump(APPB_ADDR);
		}	
		if(pConfig->boot_target == BOOT_LOAD_APP_B && pConfig->ota_state == OTA_STATUS_NORMAL)
		{
			Serial_Printf("B区程序正常,跳转至B区\r\n");
			APP_Jump(APPB_ADDR);
		}
		if(pConfig->boot_target == BOOT_LOAD_APP_A && pConfig->ota_state == OTA_STATUS_NORMAL)
		{
			Serial_Printf("A区程序正常,跳转至A区\r\n");
			APP_Jump(APPA_ADDR);
		}
		
//////////////////////////////////////////////////////////////////////////////////////
//		IWDG_ReloadCounter();//喂狗
//		if(KEY_Read() == 0)//按下按键，进入Ymodem升级模式
//		{
//			ymodem_start();//只能调用一次
//			while(1)
//			{
//				int status = ymodem_recv_process();
//				if(status<0)
//				{
//					Serial_Printf("Config Error\r\n");
//					while(1);
//				}
//				
//				if(Transfer_Done_Flag)
//				{
//					Serial_Printf("Jump to APP\r\n");
//					Transfer_Done_Flag = 0;
//					NVIC_SystemReset();
//				}
//			}
//		}

			
////////////////原先串口空闲中断＋DMA方案，由于Ymodem的加入，改为了轮询模式/////////////////////////		
//		if(DMA_RecvFlag)
//		{
//			DMA_RecvFlag = 0;
//			USART1_Recvlen = 1024 - DMA_GetCurrDataCounter(DMA1_Channel5);
//			DMA_Cmd(DMA1_Channel5,DISABLE);
//			DMA_SetCurrDataCounter(DMA1_Channel5, 1024); 
//			DMA_Cmd(DMA1_Channel5,ENABLE);
//			if(USART1_Recvlen>=1024)
//			{
//				Serial_Printf("DMA串口1接收数据溢出\r\n");
//			}
//			else
//			{
//				DMA_USART1Buf[USART1_Recvlen] = '\0';
//				Serial_Printf("DMA串口1接收数据长度为%d\r\n",USART1_Recvlen);
//				for(uint32_t i = 0;i<USART1_Recvlen;i+=2)
//				{
//					temp_data = (uint16_t)((DMA_USART1Buf[i+1]<<8)|DMA_USART1Buf[i]);
//					flash_unlock();
//					flash_WriteHalfWord(APP_ADDR+i,temp_data);
//					flash_lock();
//				}
//				Serial_Printf("Flash写入数据完成\r\n");
//			}
//			memset(DMA_USART1Buf,0xff,DMA_BUFSIZE);	
		
///////////////////////////////////////////////////////////////////////////////////////////////
	}
		 
   
}


