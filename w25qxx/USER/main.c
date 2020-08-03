#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h"
#include "oled_hw_spi.h"
#include "bmp.h"

//SPIͨ�Žӿ�ʵ��-�⺯���汾
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com

//Ҫд�뵽W25Q16���ַ�������
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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����ϵͳ�ж����ȼ�����2
    delay_init(168);                                //��ʼ����ʱ����
    uart_init(115200);                              //��ʼ�����ڲ�����Ϊ115200
    LED_Init();                                     //��ʼ��LED
    // LCD_Init();                                     //LCD��ʼ��
    KEY_Init();    //������ʼ��
    W25QXX_Init(); //W25QXX��ʼ��
    SPI2_Init();
    oled_init();
    // POINT_COLOR = RED;
    // LCD_ShowString(30, 50, 200, 16, 16, "mcudev STM32F4");
    // LCD_ShowString(30, 70, 200, 16, 16, "SPI TEST");
    // LCD_ShowString(30, 90, 200, 16, 16, "mcudev.taobao.com");
    // LCD_ShowString(30, 110, 200, 16, 16, "2015/8/6");
    // LCD_ShowString(30, 130, 200, 16, 16, "KEY1:Write KEY0:Read"); //��ʾ��ʾ��Ϣ
    OLED_ShowString(0, 0, "KEY1:Write KEY0:Read", 12); //��ʾ��ʾ��Ϣ
    FLASH_SIZE = 16 * 1024 * 1024;                     //FLASH ��СΪ2M�ֽ�,16M-bit

    while (W25QXX_ReadID() != W25Q16) //��ⲻ��W25Q16
    {
        // LCD_ShowString(30, 150, 200, 16, 16, "W25Q16 Check Failed!");
        OLED_ShowString(0, 12, "Check Failed!", 12);
        delay_ms(500);
        // LCD_ShowString(30, 150, 200, 16, 16, "Please Check!");
        OLED_ShowString(0, 12, "Please Check!", 12);
        delay_ms(500);
        LED0 = !LED0; //DS0��˸
    }
    // LCD_ShowString(30, 150, 200, 16, 16, "W25Q16 Ready!");
    OLED_ShowString(0, 12, "W25Q16 Ready!", 12);
    // POINT_COLOR = BLUE;            //��������Ϊ��ɫ
    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY1_PRES) //KEY1����,д��W25Q16
        {
            // LCD_Fill(0, 170, 239, 319, WHITE); //�������
            // LCD_ShowString(30, 170, 200, 16, 16, "Start Write W25Q16....");
            OLED_ShowString(0, 24, "Start Write....", 12);
            W25QXX_Write((u8 *)TEXT_Buffer, FLASH_SIZE - 100, SIZE2); //�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
            // LCD_ShowString(30, 170, 200, 16, 16, "W25Q16 Write Finished!"); //��ʾ�������
            OLED_ShowString(0, 24, "Write Finished!", 12);
        }
        if (key == KEY0_PRES) //KEY0����,��ȡ�ַ�������ʾ
        {
            // LCD_ShowString(30, 170, 200, 16, 16, "Start Read W25Q16.... ");
            OLED_ShowString(0, 24, "Start Read.... ", 12);

            W25QXX_Read(datatemp, FLASH_SIZE - 100, SIZE2); //�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
            // LCD_ShowString(30, 170, 200, 16, 16, "The Data Readed Is:   "); //��ʾ�������
            // LCD_ShowString(30, 190, 200, 16, 16, datatemp);                 //��ʾ�������ַ���
            OLED_ShowString(0, 24, "The DataIs:", 12);
            OLED_ShowString(72, 24, datatemp, 12);
            for (t = 0; t < 1; t++)
            {
                USART_SendData(USART1, datatemp[t]); //�򴮿�1��������
                while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
                    ; //�ȴ����ͽ���
            }
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
