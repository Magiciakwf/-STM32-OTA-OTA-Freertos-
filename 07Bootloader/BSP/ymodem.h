/**
 * Copyright (c) 2025 Xinkerr
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Disclaimer / 免责声明
 *
 * This software is provided "as is", without warranty of any kind, express or implied, 
 * including but not limited to the warranties of merchantability, fitness for a 
 * particular purpose, or non-infringement. In no event shall the authors or copyright 
 * holders be liable for any claim, damages, or other liability, whether in an action 
 * of contract, tort, or otherwise, arising from, out of, or in connection with the 
 * software or the use or other dealings in the software.
 *
 * 本软件按“原样”提供，不附带任何明示或暗示的担保，包括但不限于对适销性、特定用途适用性
 * 或非侵权的保证。在任何情况下，作者或版权持有人均不对因本软件或使用本软件而产生的任何
 * 索赔、损害或其他责任负责，无论是合同诉讼、侵权行为还是其他情况。
 */
 
#ifndef __YMODEM_H__
#define __YMODEM_H__

/******************************************CONFIG***************************************************/
#define YMODEM_BUF_SIZE                 150     //ymoedm存放一帧数据的buffer
#define YMODEM_INITIATE_TIMEOUT         30000   //发起请求的超时时间 单位同uptime
#define YMODEM_REC_TIMEOUT              1200     //一帧数据的接收超时时间 单位同uptime
#define YMODEM_1K_ENABLE                0       //是否使能1K模式
/***************************************************************************************************/

#include <stdint.h>
typedef void (*putc_func_t)(uint8_t put_data);
typedef int (*read_func_t)(uint8_t* pdata, int len);
typedef void (*ymodem_file_handler_t)(char* file, int size);
typedef void (*ymodem_data_handler_t)(uint8_t num, uint8_t* pdata, int len);
typedef void (*ymodem_end_handler_t)(void);
typedef void (*ymodem_error_handler_t)(int err);
typedef uint32_t (*ym_uptime_get_t)(void);
  
/**
 * @brief  Ymodem初始化
 *
 * @param[in] putc: 注册数据发送函数
 * @param[in] read: 注册读取数据函数
 * @param[in] rec_file_func: 注册文件信息帧接收回调函数
 * @param[in] rec_data_func: 注册文件数据帧接收回调函数
 * @param[in] end_func: 注册文件接收结束帧回调函数
 * @param[in] err_func: 注册ymodem接收出错回调函数
 * @param[in] uptime_get_func: 注册系统运行时间获取函数 
 *
 * @return void
 */
void ymodem_init(putc_func_t putc, read_func_t read,
                 ymodem_file_handler_t rec_file_func,
                 ymodem_data_handler_t rec_data_func,
                 ymodem_end_handler_t end_func,
                 ymodem_error_handler_t err_func,
                 ym_uptime_get_t uptime_get_func);

/**
 * @brief  Ymodem接收端协议处理
 *
 * @return int
 */
int ymodem_recv_process(void);

/**
 * @brief  Ymodem开始接收
 *
 * @return int
 */
int ymodem_start(void);

#endif
