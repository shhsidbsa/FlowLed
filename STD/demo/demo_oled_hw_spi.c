#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "oled_hw_spi.h"
#include "bmp.h"

int main()
{
    u8 t;
    //u8 len;
    //u16 times = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //延时初始化
    uart_init(115200);                              //串口初始化波特率为115200
    LED_Init();
    SPI2_Init();
    oled_init();
    t = ' ';
    const u8 str[] = "abc";
    // oled_dis_chinese(0, 0, 0);
    // oled_dis_chinese(18, 0, 1);

    //oled_dis_one_char(0, 0, '1');
    // oled_dis_one_char(15, 0, '2');
    // oled_dis_num(60, 4, 174, 3	, 24);
    // OLED_ShowString(0, 0, str, 12);
    // OLED_ShowString(2, 0, "ABC", 12);  //ABC
    // OLED_ShowString(2, 12, "ABC", 16); //ABC
    // OLED_ShowString(2, 28, "ABC", 24); //ABC

    // oled_dis_one_char(10, 0, 'W');
    //oled_dis_logo();
    while (1)
    {
        OLED_Clear();
        OLED_DrawCircle(32, 24, 24);
        OLED_Refresh_Gram();
        delay_ms(10000);

        // OLED_ShowPicture(0, 0, 128, 8, BMP1);
        // delay_ms(1000);
        // OLED_Clear();
        // OLED_ShowChinese(0, 0, 0, 16);   //中
        // OLED_ShowChinese(18, 0, 1, 16);  //景
        // OLED_ShowChinese(36, 0, 2, 16);  //园
        // OLED_ShowChinese(54, 0, 3, 16);  //电
        // OLED_ShowChinese(72, 0, 4, 16);  //子
        // OLED_ShowChinese(90, 0, 5, 16);  //科
        // OLED_ShowChinese(108, 0, 6, 16); //技
        // OLED_ShowString(8, 16, "ZHONGJINGYUAN", 16);
        // OLED_ShowString(20, 32, "2014/05/01", 16);
        // OLED_ShowString(0, 48, "ASCII:", 16);
        // OLED_ShowString(63, 48, "CODE:", 16);
        // OLED_ShowChar(48, 48, t, 16); //显示ASCII字符
        // t++;
        // if (t > '~')
        //     t = ' ';
        // OLED_ShowNum(103, 48, t, 3, 16);
        // OLED_Refresh_Gram();
        // delay_ms(1000);
        // OLED_Clear();
        // OLED_ShowChinese(0, 0, 0, 16);   //16*16 中
        // OLED_ShowChinese(16, 0, 0, 24);  //24*24 中
        // OLED_ShowChinese(24, 20, 0, 32); //32*32 中
        // OLED_ShowChinese(64, 0, 0, 64);  //64*64 中
        // OLED_Refresh_Gram();
        // delay_ms(1000);
        // OLED_Clear();
        // OLED_ShowString(0, 0, "ABC", 12);  //6*12 “ABC”
        // OLED_ShowString(0, 12, "ABC", 16); //8*16 “ABC”
        // OLED_ShowString(0, 28, "ABC", 24); //12*24 “ABC”
        // OLED_Refresh_Gram();
        // delay_ms(1000);
        // OLED_ScrollDisplay(11, 4);
        // delay_ms(10000);

        // if (USART1_RX_STA & 0x8000)
        // {
        //     len = USART1_RX_STA & 0x3fff; //得到此次接收到的数据长度
        //     printf("\r\nyour message is:\r\n");
        //     for (t = 0; t < len; t++)
        //     {
        //         USART_SendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
        //         while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
        //             ; //等待发送结束
        //     }
        //     printf("\r\n\r\n"); //插入换行
        //     USART1_RX_STA = 0;
        // }
        // else
        // {
        //     times++;
        //     if (times % 5000 == 0)
        //     {
        //         printf("\r\nALIENTEK_STM32F407_USART\r\n");
        //         printf("@ALIENTEK\r\n\r\n\r\n");
        //     }
        //     if (times % 200 == 0)
        //         printf("input data\r\n");
        //     if (times % 30 == 0)
        //         LED0 = !LED0; //闪烁LED,提示系统正在运行.
        //     delay_ms(10);
        // }
    }
}
