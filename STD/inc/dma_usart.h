#ifndef __DMA_USART_H
#define __DMA_USART_H
#include "sys.h"
#define DMA_USART1_SEND_LEN 64

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx, u32 chx, u32 par, u32 mar, u16 ndtr); //����DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr);                            //ʹ��һ��DMA����

/*������Ҫע����DMA_Cmd(DMA1_Channel4,DISABLE)������Ҫ�����ô����С֮ǰ����һ�£����򲻻���������DMA���͡�*/
#endif
