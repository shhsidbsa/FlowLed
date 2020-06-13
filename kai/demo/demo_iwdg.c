/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-12 22:48:09
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 08:56:02
 * @Description: file content
 */
#include "kai_sys.h"
#include "kai_delay.h"
#include "kai_usart.h"
#include "kai_led.h"
#include "kai_key.h"
#include "kai_iwdg.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //初始化延时函数
    LED_Init();                                     //初始化LED端口
    KEY_Init();                                     //初始化按键
    delay_ms(100);                                  //延时100ms
    IWDG_Init(4, 500);                              //预分频数为64,重载值为500,溢出时间为1s
    LED0 = 0;                                       //先点亮红灯
    while (1)
    {
        if (KEY_Scan(0) == WKUP_PRES) //如果WK_UP按下,则喂狗
        {
            IWDG_Feed(); //喂狗
        }
        delay_ms(10);
    };
}
