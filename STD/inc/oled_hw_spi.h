#ifndef __OLED_HW_SPI_H
#define __OLED_HW_SPI_H
#include "sys.h"
#define MAX_COLUMN 128
#define MAX_ROW 64
#define SIZE 16 //显示字体选择()

#define OLED_DATA 1
#define OLED_COMM 0

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

#define OLED_CS_LOW GPIO_ResetBits(OLED_PORT, OLED_CS_Pin)
#define OLED_CS_HIGH GPIO_SetBits(OLED_PORT, OLED_CS_Pin)

#define OLED_DC_LOW GPIO_ResetBits(OLED_PORT, OLED_DC_Pin)
#define OLED_DC_HIGH GPIO_SetBits(OLED_PORT, OLED_DC_Pin)

#define OLED_RST_LOW GPIO_ResetBits(OLED_PORT, OLED_RST_Pin)
#define OLED_RST_HIGH GPIO_SetBits(OLED_PORT, OLED_RST_Pin)

void SPI2_Init(void);            //初始化SPI2口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI2速度
// u8 SPI2_ReadWriteByte(u8 TxData); //SPI2总线读写一个字节
uint8_t spi_master_send_recv_byte(uint8_t spi_byte);
void OLED_GPIO_Init(void);
static void oled_write_byte(uint8_t write_byte);
static void oled_write_operate(uint8_t mode, uint8_t dat);

void oled_set_pos(uint8_t x, uint8_t y);
void oled_dis_on(void);
void oled_dis_off(void);
void oled_dis_clear(void);
void oled_dis_set(void);
void oled_dis_one_char(uint8_t x, uint8_t y, uint8_t str);
void oled_dis_str(uint8_t x, uint8_t y, uint8_t *str);
void oled_dis_picture(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *bmp);
void oled_dis_chinese(uint8_t x, uint8_t y, uint8_t no);
void oled_dis_logo(void);
uint32_t oled_pow(uint8_t m, uint8_t n);
void oled_dis_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size_num);

void oled_init(void);
void OLED_DrawPoint1(u8 x, u8 y, u8 t);
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot);
void OLED_Refresh_Gram(void);
// void oled_ShowString(uint8_t x, uint8_t y, uint8_t *str);
void OLED_Clear(void); //初始清屏

void OLED_ShowString1(u8 x, u8 y, const u8 *p, u8 size);
void OLED_ShowNum1(u8 x, u8 y, u32 num, u8 len, u8 size);
void OLED_ShowChar1(u8 x, u8 y, u8 chr, u8 size, u8 mode);
void OLED_WB(uint8_t data);
void SPI_WriteByte(unsigned char data, unsigned char cmd);
void WriteCmd(unsigned char cmd);
void WriteData(unsigned char data);

///////////////////////
///////////////////////
///////////////////////
///////////////////////
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_DrawPoint(u8 x, u8 y);
void OLED_ClearPoint(u8 x, u8 y);
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2);
void OLED_DrawCircle(u8 x, u8 y, u8 r);
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1);
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1);
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1);
void OLED_ScrollDisplay(u8 num, u8 space);
void OLED_WR_BP(u8 x, u8 y);
void OLED_ShowPicture(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]);
#endif
