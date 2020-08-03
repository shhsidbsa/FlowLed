#ifndef __U8G2_HW_SPI_H
#define __U8G2_HW_SPI_H
#include "stm32f4xx.h"

#define OLED_PORT GPIOB

#define OLED_DC_Pin GPIO_Pin_10
#define OLED_DC_GPIO_Port GPIOB

#define OLED_RST_Pin GPIO_Pin_11
#define OLED_RST_GPIO_Port GPIOB

#define OLED_CS_Pin GPIO_Pin_12
#define OLED_CS_GPIO_Port GPIOB

#define OLED_SCK_Pin GPIO_Pin_13
#define OLED_SCK_GPIO_Port GPIOB

#define OLED_MISO_Pin GPIO_Pin_14
#define OLED_MISO_GPIO_Port GPIOB

#define OLED_MOSI_Pin GPIO_Pin_15
#define OLED_MOSI_GPIO_Port GPIOB

#define OLED_DC PAout(10)
#define OLED_RST PAout(11)
#define OLED_CS PAout(12)

#endif
