/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-22 17:56:07
 * @LastEditors: kai
 * @LastEditTime: 2020-06-23 11:32:35
 * @Description: file content
 */

#ifndef __WS2812B_H
#define __WS2812B_H
#include "kai_sys.h"
#include "kai_delay.h"

void WS2812B_Init(void);
/* Exported types ------------------------------------------------------------*/
typedef struct
{
    u8 R;
    u8 G;
    u8 B;
} RGBColor_TypeDef;

/* Exported constants --------------------------------------------------------*/
#define Pixel_S1_NUM 8 //灯环-1 RGB数量

#define CODE0 0xC0 //0x80 或 0xC0
#define CODE1 0xFC //0xF8 或 0xFC

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void WS2812b_Configuration(void);

/* Basic Color Effects -------------------------------------------------------*/
void RGB_RED(u16 Pixel_LEN);
void RGB_GREEN(u16 Pixel_LEN);
void RGB_BLUE(u16 Pixel_LEN);
void RGB_YELLOW(u16 Pixel_LEN);
void RGB_MAGENTA(u16 Pixel_LEN);
void RGB_BLACK(u16 Pixel_LEN);
void RGB_WHITE(u16 Pixel_LEN);

/* Complicated Color Effects -------------------------------------------------*/
void rainbowCycle(u8 Pixel_LEN);
#endif
