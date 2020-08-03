#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "oled_sw_spi.h"
#include "bmp.h"

int main()
{
    u8 t;
    u8 len;
    u16 times = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //延时初始化
    uart_init(115200);                              //串口初始化波特率为115200
    LED_Init();
    OLEDConfiguration();
    // OLED_ShowCHinese(0, 0, 8);
    // OLED_ShowCHinese(18, 0, 9);
    // OLED_ShowCHinese(36, 0, 10);
    OLED_DrawBMP(0, 0, 127, 7, myBitmap);
    while (1)
    {
        if (USART1_RX_STA & 0x8000)
        {
            len = USART1_RX_STA & 0x3fff; //得到此次接收到的数据长度
            printf("\r\nyour message is:\r\n");
            for (t = 0; t < len; t++)
            {
                USART_SendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
                while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
                    ; //等待发送结束
            }
            printf("\r\n\r\n"); //插入换行
            USART1_RX_STA = 0;
        }
        else
        {
            times++;
            if (times % 5000 == 0)
            {
                printf("\r\nALIENTEK_STM32F407_USART\r\n");
                printf("@ALIENTEK\r\n\r\n\r\n");
            }
            if (times % 200 == 0)
                printf("input data\r\n");
            if (times % 30 == 0)
                LED0 = !LED0; //闪烁LED,提示系统正在运行.
            delay_ms(10);
        }
    }
}
