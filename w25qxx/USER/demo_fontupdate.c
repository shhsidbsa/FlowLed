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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����ϵͳ�ж����ȼ�����2
    delay_init(168);                                //��ʼ����ʱ����
    uart6_init(BAUD_RATE);                          //��ʼ�����ڲ�����Ϊ115200
    LED_Init();                                     //��ʼ��LED
    //LCD_Init();                                     //LCD��ʼ��
    KEY_Init();
    W25QXX_Init(); //��ʼ��W25Q64
    SPI2_Init();
    oled_init();

    // printf("Font no found...\r\n");
    // LCD_ShowString(20, 50, 200, 24, 24, (u8 *)"Font no found...");
    // LCD_ShowString(20, 80, 200, 24, 24, (u8 *)"Font Updating...");
    // OLED_ShowString(0, 0, "Font Updating...", 12);
    // update_font(0, 12, 12); //�����ֿ�
    // OLED_ShowString(0, 12, "Erasing sectors ", 12);
    // OLED_ShowString(114, 12, "%", 12);

    // for (i = 0; i <= 191; i++) //�Ȳ����ֿ�����,���д���ٶ�
    // {
    //     fupd_prog(96, 12, 12, 191, i); //������ʾ
    // }
    OLED_ShowString(0, 0, "1:Update,0:Show", 12);
    //
    // printf("Font is readly ...\r\n");
    // // POINT_COLOR = RED;
    // Show_Str(0, 0, "STM32F407��Ƭ��", 12);
    // Show_Str(0, 12, "2020��7��30��", 12);
    // Show_Str(0, 0, "STM��Ƭ��", 12);
    // Show_Str(30, 50 + 24 * 1, 320, 24, "���ߵ�ʵ����", 24, 0);
    // Show_Str(30, 50 + 24 * 2, 320, 24, "2019��1��23��", 24, 0);
    // POINT_COLOR = BLUE;

    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY1_PRES) //KEY1����,д��W25Q16
        {
            update_font(0, 12, 12); //�����ֿ�
            printf("Font is readly ...\r\n");
        }
        if (key == KEY0_PRES) //KEY1����,д��W25Q16
        {
            OLED_Clear();
					OLED_ShowString(0, 0, "1:Update,0:Show", 12);
            Show_Str(0, 12, "STM32F407��Ƭ��", 12);
            Show_Str(0, 24, "2020��8��1��", 12);
					Show_Str(0, 36, "�����ǽ�����", 12);
					Show_Str(0, 48, "ף������ٲ�ʢ", 12);
        }
        i++;
        delay_ms(10);
        if (i == 20)
        {
            LED0 = !LED0; //��ʾϵͳ��������
            i = 0;
        }
    }
}
