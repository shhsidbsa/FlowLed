/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-07-18 22:36:46
 * @LastEditors: kai
 * @LastEditTime: 2020-07-18 22:40:35
 * @Description: file content
 */
#ifndef __DMA_H
#define __DMA_H
#include "sys.h"

#define DMA_USART1_SEND_LEN 64
void DMA_SEND_EN(void)
{
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel4, DMA_USART1_SEND_LEN);
    DMA_Cmd(DMA1_Channel4, ENABLE);
}
/*这里需要注意下DMA_Cmd(DMA1_Channel4,DISABLE)函数需要在设置传输大小之前调用一下，否则不会重新启动DMA发送。*/
#endif
