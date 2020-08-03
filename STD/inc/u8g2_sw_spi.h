#ifndef __U8G2_SW_SPI_H
#define __U8G2_SW_SPI_H
#include "stm32f4xx.h"

#include "stm32f4xx.h"
#include "stdlib.h"
#include "delay.h"

// #define OLED 0
// #define SIZE 16
// #define XLevelL 0x00
// #define XLevelH 0x10
// #define Max_Column 128
// #define Max_Row 64
// #define Brightness 0xff
// #define X_WIDTH 128
// #define Y_WIDTH 64

//IO Definitions

#define OLED_CS_Pin GPIO_Pin_3
#define OLED_CS_GPIO_Port GPIOD

#define OLED_DC_Pin GPIO_Pin_4
#define OLED_DC_GPIO_Port GPIOD

#define OLED_RST_Pin GPIO_Pin_5
#define OLED_RST_GPIO_Port GPIOD

#define OLED_MOSI_Pin GPIO_Pin_6
#define OLED_MOSI_GPIO_Port GPIOD

#define OLED_SCK_Pin GPIO_Pin_7
#define OLED_SCK_GPIO_Port GPIOD

#define OLED_MISO_Pin GPIO_Pin_8
#define OLED_MISO_GPIO_Port GPIOD

#define OLED_PORT GPIOD

#endif
