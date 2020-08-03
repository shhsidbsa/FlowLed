/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-07 16:55:32
 * @LastEditors: kai
 * @LastEditTime: 2020-07-22 10:00:31
 * @Description: file content
 */
#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"
#include "stm32f4xx_conf.h"
#include "sys.h"

#define DMA_USART1_RECEIVE_LEN 18
#define USART_REC_LEN 200 //定义最大接收字节数 200
#define EN_USART1_RX 1    //使能（1）/禁止（0）串口1接收

#define Max_BUFF_Len 18

extern u16 Uart1_Rx;
extern u16 Uart2_Rx;
extern u8 USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u8 Uart1_Buffer[Max_BUFF_Len];
extern u8 Uart2_Buffer[Max_BUFF_Len];
extern u16 USART1_RX_STA; //接收状态标记
extern u16 USART2_RX_STA; //接收状态标记
extern u8 USART1_RECEIVE_DMABuffer[Max_BUFF_Len];
extern u8 USART2_RECEIVE_DMABuffer[Max_BUFF_Len];
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
#endif
