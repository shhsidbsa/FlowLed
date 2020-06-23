/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-22 16:51:41
 * @LastEditors: kai
 * @LastEditTime: 2020-06-23 11:35:17
 * @Description: file content
 */

#include "kai_sys.h"
#include "kai_delay.h"
#include "kai_usart.h"
#include "kai_led.h"
#include "ws2812b.h"

int main(void)
{
    delay_init(168);
    WS2812b_Configuration();
}
