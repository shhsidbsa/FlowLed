#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "oled_hw_spi.h"
#include "led.h"
// #include "spi.h"
#include "w25qxx.h"
#include "fontupdate.h"
#include "text.h"
#include "key.h"

int main(void)
{
    // u16 i;
    u8 key;
    u16 i = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //初始化延时函数
    uart6_init(BAUD_RATE);                          //初始化串口波特率为115200
    LED_Init();                                     //初始化LED
    //LCD_Init();                                     //LCD初始化
    KEY_Init();
    W25QXX_Init(); //初始化W25Q64
    SPI2_Init();
    oled_init();

    // printf("Font no found...\r\n");
    // LCD_ShowString(20, 50, 200, 24, 24, (u8 *)"Font no found...");
    // LCD_ShowString(20, 80, 200, 24, 24, (u8 *)"Font Updating...");
    // OLED_ShowString(0, 0, "Font Updating...", 12);
    // update_font(0, 12, 12); //更新字库
    // OLED_ShowString(0, 12, "Erasing sectors ", 12);
    // OLED_ShowString(114, 12, "%", 12);

    // for (i = 0; i <= 191; i++) //先擦除字库区域,提高写入速度
    // {
    //     fupd_prog(96, 12, 12, 191, i); //进度显示
    // }
    OLED_ShowString(0, 0, "1:Update,0:Show", 12);
    //
    // printf("Font is readly ...\r\n");
    // // POINT_COLOR = RED;
    // Show_Str(0, 0, "STM32F407单片机", 12);
    // Show_Str(0, 12, "2020年7月30日", 12);
    // Show_Str(0, 0, "STM单片机", 12);
    // Show_Str(30, 50 + 24 * 1, 320, 24, "无线电实验室", 24, 0);
    // Show_Str(30, 50 + 24 * 2, 320, 24, "2019年1月23日", 24, 0);
    // POINT_COLOR = BLUE;

    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY1_PRES) //KEY1按下,写入W25Q16
        {
            update_font(0, 12, 12); //更新字库
            printf("Font is readly ...\r\n");
        }
        if (key == KEY0_PRES) //KEY1按下,写入W25Q16
        {
            OLED_Clear();
					OLED_ShowString(0, 0, "1:Update,0:Show", 12);
            Show_Str(0, 12, "STM32F407单片机", 12);
            Show_Str(0, 24, "2020年8月1日", 12);
					Show_Str(0, 36, "今天是建军节", 12);
					Show_Str(0, 48, "祝祖国繁荣昌盛", 12);
        }
        i++;
        delay_ms(10);
        if (i == 20)
        {
            LED0 = !LED0; //提示系统正在运行
            i = 0;
        }
    }
}
