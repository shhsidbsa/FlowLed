#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h"
#include "oled_hw_spi.h"
#include "bmp.h"

//SPI通信接口实验-库函数版本
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com

//要写入到W25Q16的字符串数组
const u8 TEXT_Buffer[] = {"Explorer STM32F4 SPI TEST"};
#define SIZE2 sizeof(TEXT_Buffer)
u8 str[] = "W25Q16 Write Finished";

int main(void)
{
    u8 key;
    u16 i = 0;
    u8 datatemp[SIZE2];
    u32 FLASH_SIZE;
    u8 t;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //初始化延时函数
    uart_init(115200);                              //初始化串口波特率为115200
    LED_Init();                                     //初始化LED
    // LCD_Init();                                     //LCD初始化
    KEY_Init();    //按键初始化
    W25QXX_Init(); //W25QXX初始化
    SPI2_Init();
    oled_init();
    // POINT_COLOR = RED;
    // LCD_ShowString(30, 50, 200, 16, 16, "mcudev STM32F4");
    // LCD_ShowString(30, 70, 200, 16, 16, "SPI TEST");
    // LCD_ShowString(30, 90, 200, 16, 16, "mcudev.taobao.com");
    // LCD_ShowString(30, 110, 200, 16, 16, "2015/8/6");
    // LCD_ShowString(30, 130, 200, 16, 16, "KEY1:Write KEY0:Read"); //显示提示信息
    OLED_ShowString(0, 0, "KEY1:Write KEY0:Read", 12); //显示提示信息
    FLASH_SIZE = 16 * 1024 * 1024;                     //FLASH 大小为2M字节,16M-bit

    while (W25QXX_ReadID() != W25Q16) //检测不到W25Q16
    {
        // LCD_ShowString(30, 150, 200, 16, 16, "W25Q16 Check Failed!");
        OLED_ShowString(0, 12, "Check Failed!", 12);
        delay_ms(500);
        // LCD_ShowString(30, 150, 200, 16, 16, "Please Check!");
        OLED_ShowString(0, 12, "Please Check!", 12);
        delay_ms(500);
        LED0 = !LED0; //DS0闪烁
    }
    // LCD_ShowString(30, 150, 200, 16, 16, "W25Q16 Ready!");
    OLED_ShowString(0, 12, "W25Q16 Ready!", 12);
    // POINT_COLOR = BLUE;            //设置字体为蓝色
    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY1_PRES) //KEY1按下,写入W25Q16
        {
            // LCD_Fill(0, 170, 239, 319, WHITE); //清除半屏
            // LCD_ShowString(30, 170, 200, 16, 16, "Start Write W25Q16....");
            OLED_ShowString(0, 24, "Start Write....", 12);
            W25QXX_Write((u8 *)TEXT_Buffer, FLASH_SIZE - 100, SIZE2); //从倒数第100个地址处开始,写入SIZE长度的数据
            // LCD_ShowString(30, 170, 200, 16, 16, "W25Q16 Write Finished!"); //提示传送完成
            OLED_ShowString(0, 24, "Write Finished!", 12);
        }
        if (key == KEY0_PRES) //KEY0按下,读取字符串并显示
        {
            // LCD_ShowString(30, 170, 200, 16, 16, "Start Read W25Q16.... ");
            OLED_ShowString(0, 24, "Start Read.... ", 12);

            W25QXX_Read(datatemp, FLASH_SIZE - 100, SIZE2); //从倒数第100个地址处开始,读出SIZE个字节
            // LCD_ShowString(30, 170, 200, 16, 16, "The Data Readed Is:   "); //提示传送完成
            // LCD_ShowString(30, 190, 200, 16, 16, datatemp);                 //显示读到的字符串
            OLED_ShowString(0, 24, "The DataIs:", 12);
            OLED_ShowString(72, 24, datatemp, 12);
            for (t = 0; t < 1; t++)
            {
                USART_SendData(USART1, datatemp[t]); //向串口1发送数据
                while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
                    ; //等待发送结束
            }
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
