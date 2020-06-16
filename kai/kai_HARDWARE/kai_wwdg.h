/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-13 09:24:46
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 10:53:01
 * @Description: file content
 */

#ifndef __KAI_WWDG_H
#define __KAI_WWDG_H

#include "kai_sys.h"

void WWDG_Init(u8 tr, u8 wr, u32 fprer);
void WWDG_IRQHandler(void);
#endif
