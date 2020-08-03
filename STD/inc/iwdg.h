/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-12 22:44:57
 * @LastEditors: kai
 * @LastEditTime: 2020-06-13 08:42:19
 * @Description: file content
 */

#ifndef __IWDG_H
#define __IWDG_H
#include "sys.h"

void IWDG_Init(u8 prer, u16 rlr); //IWDG初始化
void IWDG_Feed(void);             //喂狗函数
#endif
