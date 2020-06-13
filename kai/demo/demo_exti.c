/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-11 22:04:11
 * @LastEditors: kai
 * @LastEditTime: 2020-06-12 10:12:10
 * @Description: file content
 */

#include "kai_sys.h"
#include "kai_delay.h"
#include "kai_usart.h"
#include "kai_led.h"
#include "kai_key.h"
#include "kai_exti.h"

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组 2
    delay_init(168);                                //初始化延时函数
    uart_init(115200);                              //串口初始化
    LED_Init();                                     //初始化 LED 端口
    EXTIX_Init();                                   //初始化外部中断输入
    LED0 = 0;                                       //先点亮红灯
    while (1)
    {
        printf("OK\r\n"); //打印 OK 提示程序运行
        delay_ms(1000);   //每隔 1s 打印一次
    }
}
