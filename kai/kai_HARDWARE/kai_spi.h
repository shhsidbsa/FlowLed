#ifndef __KAI_SPI_H
#define __KAI_SPI_H
#include "kai_sys.h"

 	    													  
void SPI1_Init(void);			 //��ʼ��SPI1��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�
		 
#endif

