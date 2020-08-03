#ifndef __DMA_USART_H
#define __DMA_USART_H
#include "sys.h"
#define DMA_USART1_SEND_LEN 64

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx, u32 chx, u32 par, u32 mar, u16 ndtr); //配置DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr);                            //使能一次DMA传输

/*这里需要注意下DMA_Cmd(DMA1_Channel4,DISABLE)函数需要在设置传输大小之前调用一下，否则不会重新启动DMA发送。*/
#endif
