/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-13 09:52:19
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 09:52:39
 * @Description: file content
 */
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "wwdg.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //初始化延时函数
    LED_Init();                                     //初始化LED端口
    LED0 = 0;                                       //点亮LED0
    delay_ms(1000);
    WWDG_Init(0x7F, 0X5F, WWDG_Prescaler_8); //计数器值为7f,窗口寄存器为5f,分频数为8

    while (1)
    {
        LED0 = 1; //熄灭LED灯
    }
}
