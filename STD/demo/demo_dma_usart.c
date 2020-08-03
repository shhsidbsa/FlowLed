#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "oled_hw_spi.h"
#include "key.h"
#include "dma_usart.h"

#define SEND_BUF_SIZE 8200 //�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.

u8 SendBuff[SEND_BUF_SIZE]; //�������ݻ�����
const u8 TEXT_TO_SEND[] = {"Mcudev STM32F4 DMA ����ʵ��"};

int main(void)
{
    u16 i;
    u8 t = 0;
    u8 j, mask = 0;
    float pro = 0;                                  //����
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����ϵͳ�ж����ȼ�����2
    delay_init(168);                                //��ʼ����ʱ����
    uart_init(115200);                              //��ʼ�����ڲ�����Ϊ115200
    LED_Init();                                     //��ʼ��LED
    KEY_Init();                                     //������ʼ��
    SPI2_Init();
    oled_init();
    OLED_ShowString(0, 0, "STM32F4 DMA TEST", 12);
    OLED_Refresh_Gram();

    MYDMA_Config(DMA2_Stream7, DMA_Channel_4, (u32)&USART1->DR,
                 (u32)SendBuff, SEND_BUF_SIZE); //DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
    //��ʾ��ʾ��Ϣ
    j = sizeof(TEXT_TO_SEND);
    for (i = 0; i < SEND_BUF_SIZE; i++) //���ASCII�ַ�������
    {
        if (t >= j) //���뻻�з�
        {
            if (mask)
            {
                SendBuff[i] = 0x0a;
                t = 0;
            }
            else
            {
                SendBuff[i] = 0x0d;
                mask++;
            }
        }
        else //����TEXT_TO_SEND���
        {
            mask = 0;
            SendBuff[i] = TEXT_TO_SEND[t];
            t++;
        }
    }
    i = 0;
    while (1)
    {
        t = KEY_Scan(0);
        if (t == KEY0_PRES) //KEY0����
        {
            printf("\r\nDMA DATA:\r\n");
            // LCD_ShowString(30, 150, 200, 16, 16, "Start Transimit....");
            // LCD_ShowString(30, 170, 200, 16, 16, "   %");  //��ʾ�ٷֺ�
            OLED_ShowString(0, 12, "Start Transimit....", 12);
            OLED_ShowString(0, 24, "   %", 12);
            OLED_Refresh_Gram();

            USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���1��DMA����
            MYDMA_Enable(DMA2_Stream7, SEND_BUF_SIZE);     //��ʼһ��DMA���䣡
            //�ȴ�DMA������ɣ���ʱ������������һЩ�£����
            //ʵ��Ӧ���У����������ڼ䣬����ִ�����������
            while (1)
            {
                if (DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) != RESET) //�ȴ�DMA2_Steam7�������
                {
                    DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7); //���DMA2_Steam7������ɱ�־
                    break;
                }
                pro = DMA_GetCurrDataCounter(DMA2_Stream7); //�õ���ǰ��ʣ����ٸ�����
                pro = 1 - pro / SEND_BUF_SIZE;              //�õ��ٷֱ�
                pro *= 100;                                 //����100��
                // LCD_ShowNum(30, 170, pro, 3, 16);
                OLED_ShowNum(0, 24, pro, 3, 12);
                OLED_Refresh_Gram();
            }
            // LCD_ShowNum(30, 170, 100, 3, 16);                            //��ʾ100%
            OLED_ShowNum(0, 24, 100, 3, 12);
            // LCD_ShowString(30, 150, 200, 16, 16, "Transimit Finished!"); //��ʾ�������
            OLED_ShowString(0, 12, "Transimit Finished!", 12);
            OLED_Refresh_Gram();
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
