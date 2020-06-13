/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-07 10:29:00
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 08:57:29
 * @Description: PC通过串口发送数据，stm32返回相同数据
 */

//不要编译kai_usart
#include "kai_sys.h"

void My_USART1_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef UASRT_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //使能串口1时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //使能GPIOA时钟
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    //GPIOF9,F10初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //复用模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    UASRT_InitStructure.USART_BaudRate = 115200;
    UASRT_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    UASRT_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    UASRT_InitStructure.USART_Parity = USART_Parity_No;
    UASRT_InitStructure.USART_StopBits = USART_StopBits_1;
    UASRT_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &UASRT_InitStructure);
    USART_Cmd(USART1, ENABLE);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //USART_IT_RXNE收到数据就产生中断

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
}

//中断服务函数
void USART1_IRQHandler()
{
    u8 res;
    if (USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        res = USART_ReceiveData(USART1);
        USART_SendData(USART1, res);
    }
}

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //两位抢占优先级，两位响应优先级
    My_USART1_Init();
    while (1)
        ;
}
