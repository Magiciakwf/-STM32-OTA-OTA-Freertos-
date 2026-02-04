#include "user_flash.h"
#include "ota.h"
#include "ota_cfg.h"
#include "flash.h"
#include "CAN_Tp.h"
#include "Serial.h"

uint32_t move_addr = 0;
uint32_t target_addr = 0;

void user_flash_erase(void)//必须发送擦除指令才会调用这个函数，调用这个函数后target_addr才会被赋值
{
	if(get_current_slot() == 1)
	{
		target_addr = APPB_ADDR;
		Serial_Printf("擦除目标为B区\r\n");//
	}
	else
	{
		target_addr = APPA_ADDR;
		Serial_Printf("擦除目标为A区\r\n");
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//确保数据物理上已经发出去
	flash_unlock();
	for (int i = 0;i<FLASH_PAGENUM;i++)
	{
//		taskENTER_CRITICAL();
		flash_ErasePage(target_addr+i*2048);//ZET6每页为2k
//		taskEXIT_CRITICAL();
//		vTaskDelay(1);//没擦除一页让出CPU一毫秒，提高系统的实时响应
	}//Flash擦除过程中中断无法执行
	flash_lock();

}

void user_erase_start(void)
{
		move_addr = 0;
		user_flash_erase();//收到FF帧，开始擦除数据
}

//void user_flash_write(void)
//{
//	uint32_t target_addr;
//	uint16_t data = 0;
//	if(get_current_slot() == 1)target_addr = APPB_ADDR;
//	else target_addr = APPA_ADDR;	
//	
//	if(Recv_len<256)
//	{
//		if(Recv_len%2!=0)
//		{
//			for (int i = 0;i<(Recv_len/2+1);i++)
//			{
//				if(i!=Recv_len/2)
//				{
//					data = (uint16_t)CANTP_RecvBuf[2*i]<<8 | CANTP_RecvBuf[2*i+1];
//					flash_WriteHalfWord(target_addr + i*2 + move_addr,data);//高八位写在target_addr+1，低八位写在target_addr
//				}
//				else
//				{
//					data = (uint16_t)CANTP_RecvBuf[2*i] &0xFF00;
//					flash_WriteHalfWord(target_addr + i*2 + move_addr,data);
//				}
//			}
//		}
//		else
//		{
//			for (int i = 0;i<(Recv_len/2);i++)
//			{
//				data = (uint16_t)CANTP_RecvBuf[2*i]<<8 | CANTP_RecvBuf[2*i + 1];
//				flash_WriteHalfWord(target_addr + i*2 + move_addr,data);
//			}
//		}
//	}
//	else
//	{
//			flash_unlock();
//			for (int i = 0;i<(Recv_len/2);i++)
//			{
//				data = (uint16_t)CANTP_RecvBuf[2*i]<<8 | CANTP_RecvBuf[2*i + 1];
//				flash_WriteHalfWord(target_addr + i*2 + move_addr,data);
//			}		
//			flash_lock();
//			move_addr += 256;
//	}
//	
//}

//小端模式：先来的数据放低位
void user_flash_write(uint32_t len,uint8_t *pBuf)
{
    uint16_t data;
    // ... (地址获取和解锁保持不变) ...

    uint16_t half_cnt = len / 2;
	
		flash_unlock();

    for (uint16_t i = 0; i < half_cnt; i++)
    {
        // 【关键修复3】适配小端模式
        // 原代码: data = High<<8 | Low (这是把先收到的放在高8位，写入后在内存高地址)
        // 修改后: data = Low | High<<8 (这是把先收到的放在低8位，写入后在内存低地址)
        
        // 假设 buffer 顺序是 B0 B1
        // 我们希望内存里是 B0(addr) B1(addr+1)
        // 16位写操作: 值应为 0xB1B0
        data = (uint16_t)pBuf[2*i] | ((uint16_t)pBuf[2*i + 1] << 8);

        flash_WriteHalfWord(target_addr + move_addr + i * 2, data);
    }

    /* 处理剩余 1 字节 */
    if (len & 0x01)
    {
        // 【关键修复4】奇数尾巴处理
        // 原代码把数据放在高8位(0xXX00)，写入会导致内存变成 00 XX (低位填了0)
        // 应该把数据放在低8位(0x00XX)，写入会导致内存变成 XX 00 (高位填了0)
        // 注意：00 是否会覆盖下一个字节取决于你的 Flash 写入机制，通常为了安全，padding 0xFF 更好
        data = (uint16_t)pBuf[len - 1]; // 放在低8位
        
        // 建议：如果Flash默认擦除值是0xFF，这里高8位最好填0xFF，避免破坏下一个字节（虽然通常按顺序写没问题）
        data |= 0xFF00; 

        flash_WriteHalfWord(target_addr + move_addr + half_cnt * 2, data);
    }

    flash_lock();
    move_addr += len;
}

//void user_flash_write(void)
//{
//    uint32_t target_addr;
//    uint16_t data;

//    target_addr = (get_current_slot() == 1) ? APPB_ADDR : APPA_ADDR;

//    flash_unlock();

//    uint16_t half_cnt = Recv_len / 2;

//    for (uint16_t i = 0; i < half_cnt; i++)
//    {
//        data = ((uint16_t)CANTP_RecvBuf[2*i] << 8)
//             |  (uint16_t)CANTP_RecvBuf[2*i + 1];

//        flash_WriteHalfWord(target_addr + move_addr + i * 2, data);
//    }

//    /* 处理剩余 1 字节 */
//    if (Recv_len & 0x01)
//    {
//        data = ((uint16_t)CANTP_RecvBuf[Recv_len - 1]) << 8;
//        flash_WriteHalfWord(target_addr + move_addr + half_cnt * 2, data);
//    }

//    flash_lock();

//    move_addr += Recv_len;
//}
