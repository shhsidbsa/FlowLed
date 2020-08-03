#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h"
#include "oled_hw_spi.h"
// const u8 TEXT_Buffer[] = {"fc04fc00fe42be00f212f202fe0200000f040f00ff100f000f044f807f00000000fe0222da0600f21212f202fe02020000ff08100807000f04044f807f0000002020ff20201098f4929190949830000010301f0888854424140f1424448484001010ff10001098f4929190949830000042827f0184854424140f142444848400"};
// #define SIZE12 sizeof(TEXT_Buffer)
int main(void)
{
    u8 key;
    uint32_t WriteAddress = 0;
    u16 i = 0;
    u16 times = 0;
    u8 t;
    u16 len;
    // u8 datatemp[SIZE12];
    u32 FLASH_SIZE;
    uint8_t str[] = "啊阿中碍爱鞍昂轼軎戤愿岳";
    u8 kai[8] = {0};
    u32 kai_addr = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //初始化延时函数
    uart_init(115200);                              //初始化串口波特率为115200
    LED_Init();                                     //初始化LED
    // LCD_Init();                                     //LCD初始化
    KEY_Init();    //按键初始化
    W25QXX_Init(); //W25QXX初始化
    SPI2_Init();
    oled_init();
    // while (W25QXX_ReadID() != W25Q16) //检测不到W25Q16
    // {
    //     // LCD_ShowString(30, 150, 200, 16, 16, "W25Q16 Check Failed!");
    //     OLED_ShowString(0, 12, "Check Failed!", 12);
    //     delay_ms(500);
    //     // LCD_ShowString(30, 150, 200, 16, 16, "Please Check!");
    //     OLED_ShowString(0, 12, "Please Check!", 12);
    //     delay_ms(500);
    //     LED0 = !LED0; //DS0闪烁
    // }
    // W25QXX_Write((u8 *)TEXT_Buffer, 0, SIZE12); //从倒数第100个地址处开始,写入SIZE长度的数据
    // W25QXX_Erase_Chip();
    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY1_PRES) //KEY1按下,写入W25Q16
        {
            W25QXX_Read(kai, kai_addr, 8);
            kai_addr += 8;
            // printf((const char *)kai);
            for (t = 0; t < 1; t++)
            {
                USART_SendData(USART1, kai[t]); //向串口1发送数据
                while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
                    ; //等待发送结束
            }
            printf("\r\n"); //插入换行
        }
        if (key == KEY0_PRES) //KEY0按下,读取字符串并显示
        {
            GUI_Chinese(0, 0, str, 16);
        }
        i++;
        delay_ms(10);
        if (i == 20)
        {
            LED0 = !LED0; //提示系统正在运行
            i = 0;
        }
        // if (USART_RX_STA & 0x8000)
        // {
        //     len = USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
        //     printf("\r\nlen is:%d\r\n", len);
        //     W25QXX_Write(USART_RX_BUF, WriteAddress, len);

        //     // for (t = 0; t < len; t++)
        //     // {
        //     //     USART_SendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
        //     //     while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
        //     //         ; //等待发送结束
        //     // }
        //     // printf("\r\n\r\n"); //插入换行
        //     USART_RX_STA = 0;
        // }
        // else
        // {
        //     times++;
        //     // if (times % 5000 == 0)
        //     // {
        //     //     printf("\r\nALIENTEK_STM32F407_USART\r\n");
        //     //     printf("@ALIENTEK\r\n\r\n\r\n");
        //     // }
        //     // if (times % 200 == 0)
        //     //     printf("input data\r\n");
        //     if (times % 30 == 0)
        //         LED0 = !LED0; //闪烁LED,提示系统正在运行.
        //     delay_ms(10);
        // }
    }
}
