#ifndef __KAI_DELAY_H
#define __KAI_DELAY_H
#include <kai_sys.h>

void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
