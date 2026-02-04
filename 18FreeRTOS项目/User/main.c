#include "stm32f10x.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "sys.h"
#include "Serial.h"
#include "start_task.h"


int  main()
{
	
	__enable_irq();//前面进入后中断会被禁用
	xTaskCreate(start_task,"start_task",128*1,NULL,0,NULL);
	vTaskStartScheduler();
	
  while(1)
	{
		

	}
		 
   
}


