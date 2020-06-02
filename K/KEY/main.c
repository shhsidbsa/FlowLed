#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"

int main()
{
    u8 key;
    u8 key;          //保存键值
    delay_init(168); //初始化延时函数
    LED_Init();      //初始化 LED 端口
    BEEP_Init();     //初始化蜂鸣器端口
    KEY_Init();      //初始化与按键连接的硬件接口
    LED0 = 0;        //先点亮红灯
    while (1)
    {
        key = KEY_Scan(0); //得到键值
        if (key)
        {
            switch (key)
            {
            case WKUP_PRES: //控制蜂鸣器
                BEEP = !BEEP;
                break;
            case KEY0_PRES: //控制 LED0 翻转
                LED0 = !LED0;
                break;
            case KEY1_PRES: //控制 LED1 翻转
                LED1 = !LED1;
                break;
            case KEY2_PRES: //同时控制 LED0,LED1 翻转
                LED0 = !LED0;
                LED1 = !LED1;
                break;
            }
        }
        else
            delay_ms(10);
    }
}