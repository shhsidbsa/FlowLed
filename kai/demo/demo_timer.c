/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-13 12:14:59
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 12:19:24
 * @Description: file content
 */
#include "kai_sys.h"
#include "kai_delay.h"
#include "kai_usart.h"
#include "kai_led.h"
#include "kai_timer.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //初始化延时函数
    LED_Init();                                     //初始化LED端口

    TIM3_Int_Init(5000 - 1, 8400 - 1); //定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数5000次为500ms
    while (1)
    {
        LED0 = !LED0;  //DS0翻转
        delay_ms(200); //延时200ms
    };
}
