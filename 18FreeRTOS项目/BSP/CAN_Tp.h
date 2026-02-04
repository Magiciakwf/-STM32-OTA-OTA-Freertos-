#ifndef _CAN_TP_H_
#define _CAN_TP_H_

#include "stm32f10x.h"
#include "main.h"

#define CANTP_RECVSIZE 512

typedef enum{
	IDLE = 1,
	BUSY = 2,
	DONE = 3,
}CANTP_RecvStatus_t;

typedef enum{
	START = 1,
	END = 2,
	DATA = 3,
	
	ERASE = 4,
	WRITE= 5,
}Msg_type_t;

typedef struct{
	uint8_t type;//采用发送标志位的方式
	uint32_t Recv_len;
	union{
		uint32_t file_size;
		uint8_t CANTP_RecvBuf[CANTP_RECVSIZE];
		uint8_t crc_buf[4];
	}data;
}OTA_Msg_t;

typedef struct{
	uint8_t cmd;
	uint8_t write_buf[CANTP_RECVSIZE];
	uint32_t Recv_len;
}Job_t;

uint8_t CAN_TP_Recv(void);


extern uint8_t status;
extern uint32_t Recv_len;


#endif