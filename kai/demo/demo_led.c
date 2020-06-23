#include "kai_sys.h"
#include "kai_delay.h"
#include "kai_usart.h"
#include "kai_led.h"

//ALIENTEK ̽����STM32F407������ ʵ��1
//������ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//�������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

int main(void)
{ 
 
	delay_init(168);		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
	
  /**������ͨ��ֱ�Ӳ����⺯���ķ�ʽʵ��IO����**/	
	
	while(1)
	{
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);  //LED0��Ӧ����GPIOF.9���ͣ���  ��ͬLED0=0;
	GPIO_SetBits(GPIOA,GPIO_Pin_7);   //LED1��Ӧ����GPIOF.10���ߣ��� ��ͬLED1=1;
	delay_ms(500);  		   //��ʱ300ms
	GPIO_SetBits(GPIOA,GPIO_Pin_6);	   //LED0��Ӧ����GPIOF.0���ߣ���  ��ͬLED0=1;
	GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LED1��Ӧ����GPIOF.10���ͣ��� ��ͬLED1=0;
	delay_ms(500);                     //��ʱ300ms
	}
}


/**
*******************����ע�͵��Ĵ�����ͨ�� λ�� ����ʵ��IO�ڿ���**************************************
	
int main(void)
{ 
 
	delay_init(168);		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
  while(1)
	{
     LED0=0;			  //LED0��
	   LED1=1;				//LED1��
		 delay_ms(500);
		 LED0=1;				//LED0��
		 LED1=0;				//LED1��
		 delay_ms(500);
	 }
}
**************************************************************************************************
 **/	
	
/**
*******************����ע�͵��Ĵ�����ͨ�� ֱ�Ӳ����Ĵ��� ��ʽʵ��IO�ڿ���**************************************
int main(void)
{ 
 
	delay_init(168);		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
	while(1)
	{
     GPIOF->BSRRH=GPIO_Pin_9;//LED0��
	   GPIOF->BSRRL=GPIO_Pin_10;//LED1��
		 delay_ms(500);
     GPIOF->BSRRL=GPIO_Pin_9;//LED0��
	   GPIOF->BSRRH=GPIO_Pin_10;//LED1��
		 delay_ms(500);

	 }
 }	 
**************************************************************************************************
**/	
 


