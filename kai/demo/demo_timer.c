/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-13 12:14:59
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 12:19:24
 * @Description: file content
 */
#include "kai_sys.h"
#include "kai_delay.h"
#include "kai_usart.h"
#include "kai_led.h"
#include "kai_timer.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����ϵͳ�ж����ȼ�����2
    delay_init(168);                                //��ʼ����ʱ����
    LED_Init();                                     //��ʼ��LED�˿�

    TIM3_Int_Init(5000 - 1, 8400 - 1); //��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms
    while (1)
    {
        LED0 = !LED0;  //DS0��ת
        delay_ms(200); //��ʱ200ms
    };
}
