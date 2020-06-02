#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"

int main()
{
    delay_init(168);
    LED_Init();
    BEEP_Init();

    while (1)
    {
        GPIO_ResetBits(GPIOF, GPIO_Pin_9);
        GPIO_ResetBits(GPIOF, GPIO_Pin_8);
        delay_ms(300);
        GPIO_SetBits(GPIOF, GPIO_Pin_9);
        GPIO_SetBits(GPIOF, GPIO_Pin_8);
        delay_ms(300);
    }
}