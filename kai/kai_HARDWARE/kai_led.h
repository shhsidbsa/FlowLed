#ifndef __KAI_LED_H
#define __KAI_LED_H
#include "kai_sys.h"

//LED端口定义
#define LED0 PAout(6) // DS0
#define LED1 PAout(7) // DS1

void LED_Init(void); //初始化
#endif
