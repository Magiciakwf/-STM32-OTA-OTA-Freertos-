#ifndef START_TASK_H
#define START_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "queue.h"
#include "semphr.h" 

void start_task(void *pvParameters);

extern QueueHandle_t OTA_Queue;
extern QueueHandle_t Job_Queue;

extern SemaphoreHandle_t PrintfMutex;


#endif