/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-13 09:24:40
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 09:53:36
 * @Description: file content
 */
#include "kai_wwdg.h"
#include "kai_led.h"

//保存WWDG计数器的设置值,默认为最大.
u8 WWDG_CNT = 0X7F;

/**
 * @description: 初始化窗口看门狗
 * @param tr   :T[6:0],计数器值
 * @param wr   :W[6:0],窗口值
 * @param fprer:分频系数（WDGTB）,仅最低2位有效
 * @param Fwwdg:PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz} 
 */

void WWDG_Init(u8 tr, u8 wr, u32 fprer)
{

    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE); //使能窗口看门狗时钟

    WWDG_CNT = tr & WWDG_CNT; //初始化WWDG_CNT.
    WWDG_SetPrescaler(fprer); //设置分频值
    WWDG_SetWindowValue(wr);  //设置窗口值

    WWDG_Enable(WWDG_CNT); //开启看门狗

    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;              //窗口看门狗中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级为2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;        //子优先级为3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能窗口看门狗
    NVIC_Init(&NVIC_InitStructure);

    WWDG_ClearFlag(); //清除提前唤醒中断标志位
    WWDG_EnableIT();  //开启提前唤醒中断
}

//窗口看门狗中断服务程序
void WWDG_IRQHandler(void)
{
    WWDG_SetCounter(WWDG_CNT); //重设窗口看门狗值
    WWDG_ClearFlag();          //清除提前唤醒中断标志位
    LED1 = !LED1;
}
