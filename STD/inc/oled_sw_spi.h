#ifndef __OLED_SW_SPI_H
#define __OLED_SW_SPI_H
#include "stm32f4xx.h"
#include "stdlib.h"
#include "delay.h"

#define OLED 0
#define SIZE 16
#define XLevelL 0x00
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xff
#define X_WIDTH 128
#define Y_WIDTH 64

//IO Definitions
#define CS PDout(3)  //CS
#define DC PDout(4)  //DC
#define RST PDout(5) //RES
#define DIN PDout(6) //D1
#define CLK PDout(7) //D0

#define OLED_CS_Clr() CS = 0 //CS
#define OLED_CS_Set() CS = 1

#define OLED_RST_Clr() RST = 0 //RES
#define OLED_RST_Set() RST = 1

#define OLED_DC_Clr() DC = 0 //DC
#define OLED_DC_Set() DC = 1

#define OLED_SCLK_Clr() CLK = 0 //CLK--D0
#define OLED_SCLK_Set() CLK = 1

#define OLED_SDIN_Clr() DIN = 0 //DIN--D1
#define OLED_SDIN_Set() DIN = 1

typedef enum
{
    OLED_CMD,
    OLED_DATA
} OLED_WR_MODE;

typedef enum
{
    Display_ON,
    Display_OFF,
    Display_Clear,
    Display_Test
} DIS_MODE;

//OLED控制用函数
void OLED_WR_Byte(uint8_t dat, OLED_WR_MODE cmd);
void OLED_Display_Status(DIS_MODE mode);
void OLEDConfiguration(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowNum(uint8_t x, uint8_t y, u32 num, uint8_t len, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *p);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]);
#endif
