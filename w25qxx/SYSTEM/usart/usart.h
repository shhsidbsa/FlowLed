#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "stm32f4xx_conf.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////

//����1��ʼ��
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com
//********************************************************************************
//V1.3�޸�˵��
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//////////////////////////////////////////////////////////////////////////////////
#define USART_REC_LEN 300 //�����������ֽ��� 200
#define EN_USART1_RX 1    //ʹ�ܣ�1��/��ֹ��0������1����

extern u8 USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART_RX_STA;               //����״̬���
extern u16 USART2_RX_STA;              //����״̬���

//usart6
#define Usart6_DMA_Len 4096 //����DMA����������
#define BAUD_RATE 128000
#define WATE_TIME Usart6_DMA_Len * 10000 / BAUD_RATE //���ڷ���Usart6_DMA_Len�����ֽ�����Ҫ��ʱ�䣬��λ��ms
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr);
void uart6_init(u32 bound);
void send(u8 ch);

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
#endif
