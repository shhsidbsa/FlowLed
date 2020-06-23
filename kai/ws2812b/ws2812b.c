/*
 * @Coding: utf-8
 * @Author: kai
 * @Date: 2020-06-20 21:54:45
 * @LastEditors: kai
 * @LastEditTime: 2020-06-23 11:50:14
 * @Description: file content
 */

/*--------使用STM32F4的SPI1和SPI2分别驱动21个级联的WS2812B小灯---------*/
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------*/

// SPI初始化：
// 已知主频为168Mhz，APB2的频率为84Mhz，84/32=2.6Mhz，一个时钟周期为384ns，两个时钟周期为768ns，符合芯片的传输时间范围。
// （这里有一个坑，就是SPI1和SPI2的时钟分别来源于APB2和APB1，需要注意两个时钟的频率，两个SPI的配置不能完全相同）

//SPI口初始化
//这里针是对SPI1的初始化
// void SPI1_Init(void)
// {
//     GPIO_InitTypeDef GPIO_InitStructure;
//     SPI_InitTypeDef SPI_InitStructure;

//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  //使能SPI1时钟

//     //GPIOFB3,4,5初始化设置
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //PB3,4,5复用功能输出,3-SCK脚 4-MISO脚 5-MOSI脚
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                        //复用功能
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                      //推挽输出
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                   //100MHz
//     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                        //上拉
//     GPIO_Init(GPIOB, &GPIO_InitStructure);                              //初始化

//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

//     SPI_I2S_DeInit(SPI1);
//     SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:
//     SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       //设置SPI工作模式:设置为主SPI
//     SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   //设置SPI的数据大小:SPI发送接收8位帧结构
//     SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          //串行同步时钟的空闲状态为低电平
//     SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        //串行同步时钟的第一个跳变沿（上升或下降）数据被采样
//     SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部 NSS信号由SSI位控制
//     SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //84/32=2.6Mhz  一个时钟周期时间为384ns
//     SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
//     SPI_InitStructure.SPI_CRCPolynomial = 7;                            //CRC值计算的多项式
//     SPI_Init(SPI1, &SPI_InitStructure);                                 //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

//     SPI_CalculateCRC(SPI1, DISABLE);
//     SPI_Cmd(SPI1, ENABLE); //使能SPI外设
// }
/*-------------------------------------------------------
//这里针是对SPI2的初始化
void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //使能SPI2时钟

    //GPIO PC3初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;          //PB5复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);             //初始化

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2); //PC3复用为 SPI2

    //这里只针对SPI口初始化
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);  //复位SPI2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE); //停止复位SPI2

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          //串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        //串行同步时钟的第一个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                            //CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);                                 //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE); //使能SPI外设
}
-------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "ws2812b.h"

/* Private typedef -----------------------------------------------------------*/
/* Some Static Colors --------------------------------------------------------*/
const RGBColor_TypeDef RED = {255, 0, 0}; //全为最大亮度
const RGBColor_TypeDef GREEN = {0, 255, 0};
const RGBColor_TypeDef BLUE = {0, 0, 255};
const RGBColor_TypeDef YELLOW = {255, 255, 0};
const RGBColor_TypeDef MAGENTA = {255, 0, 255};
const RGBColor_TypeDef BLACK = {0, 0, 0};
const RGBColor_TypeDef WHITE = {255, 255, 255};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 LedsArray[(Pixel_S1_NUM)*3]; //[Gloable Variables]

/* Private functions ---------------------------------------------------------*/
/**
  * @brief      WS2812B SPI总线初始化
  * @param 
  */
void WS2812b_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    /* Enable peripheral clocks ------------------------------------------------*/
    /* GPIOA,SPI1 clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  //使能SPI1时钟

    /* Configure SPI1 pins: MOSI -----------------------------------------------*/
    /* Confugure MOSI pins as Alternate Function Push Pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); //PB3复用为 SPI1
    /* SPI1 configuration ------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; //SPI1-Tx_Only_Master
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //IDLE=High
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //SPI1CLK = 9MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7; //Default Value
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI1 CRC calculation */
    SPI_CalculateCRC(SPI1, DISABLE);

    /* Enable SPI1 */
    SPI_Cmd(SPI1, ENABLE);

    /* RGB RESET ---------------------------------------------------------------*/
    delay_ms(1);
    RGB_RED(Pixel_S1_NUM);
    delay_ms(1);
}

/**
  * @brief  Transmits one of 24 bits meassages through the SPIx peripheral.
  * @param  Data: where x can be
  *   - =0   CODE0
  *   - >0  CODE1
  * @retval None
  */
void SPI_Send_1bit(volatile u8 Data)
{
    /* Wait for SPI1 Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    /* Send SPI1 data */
    switch (Data)
    {
    case 0:
        SPI_I2S_SendData(SPI1, CODE0);
        break;
    default:
        SPI_I2S_SendData(SPI1, CODE1);
        break;
    }
}

/**
  * @brief  Config colors to a RGB pixel.
  * @param  
  * @retval None
  */
void rgb_SetColor(unsigned char LedId, RGBColor_TypeDef Color)
{
    if (LedId > Pixel_S1_NUM)
        return; //to avoid overflow
    LedsArray[LedId * 3] = Color.G;
    LedsArray[LedId * 3 + 1] = Color.R;
    LedsArray[LedId * 3 + 2] = Color.B;
}

/**
  * @brief  Apply colors to RGB pixel series.群发函数
  * @param  RGBColor_TypeDef: pointer to a RGBColor_TypeDef structure that
  *         contains the color configuration information for the RGB pixel.
  * @retval None
  */
void rgb_SendArray(void)
{
    u8 i;
    s16 j;
    for (i = 0; i < (Pixel_S1_NUM); i++)
    {
        for (j = 7; j >= 0; j--)
            SPI_Send_1bit(LedsArray[i * 3] & (1 << j));
        for (j = 7; j >= 0; j--)
            SPI_Send_1bit(LedsArray[i * 3 + 1] & (1 << j));
        for (j = 7; j >= 0; j--)
            SPI_Send_1bit(LedsArray[i * 3 + 2] & (1 << j));
    }
}

/* Basic Color Effects -------------------------------------------------------*/
/**
  * @brief  Apply red to the every pixel
  * @param  Pixel_LEN: the account of RGB pixels in serial
  * @retval None
  */
void RGB_RED(u16 Pixel_LEN)
{
    u8 i;
    for (i = 0; i < Pixel_LEN; i++)
        rgb_SetColor(i, RED);

    rgb_SendArray();
}

/**
  * @brief  Apply green to the every pixel
  * @param  Pixel_LEN: the account of RGB pixels in serial
  * @retval None
  */
void RGB_GREEN(u16 Pixel_LEN)
{
    u8 i;
    for (i = 0; i < Pixel_LEN; i++)
        rgb_SetColor(i, GREEN);

    rgb_SendArray();
}

/**
  * @brief  Apply Blue to the every pixel
  * @param  Pixel_LEN: the account of RGB pixels in serial
  * @retval None
  */
void RGB_BLUE(u16 Pixel_LEN)
{
    u8 i;
    for (i = 0; i < Pixel_LEN; i++)
        rgb_SetColor(i, BLUE);

    rgb_SendArray();
}

/**
  * @brief  Apply yellow to the every pixel
  * @param  Pixel_LEN: the account of RGB pixels in serial
  * @retval None
  */
void RGB_YELLOW(u16 Pixel_LEN)
{
    u8 i;
    for (i = 0; i < Pixel_LEN; i++)
        rgb_SetColor(i, YELLOW);

    rgb_SendArray();
}

/**
  * @brief  Apply MAGENTA to the every pixel
  * @param  Pixel_LEN: the account of RGB pixels in serial
  * @retval None
  */
void RGB_MAGENTA(u16 Pixel_LEN)
{
    u8 i;
    for (i = 0; i < Pixel_LEN; i++)
        rgb_SetColor(i, MAGENTA);

    rgb_SendArray();
}

/**
  * @brief  Apply Black to the every pixel
  * @param  Pixel_LEN: the account of RGB pixels in serial
  * @retval None
  */
void RGB_BLACK(u16 Pixel_LEN)
{
    u8 i;
    for (i = 0; i < Pixel_LEN; i++)
        rgb_SetColor(i, BLACK);

    rgb_SendArray();
}

/**
  * @brief  Apply White to the every pixel
  * @param  Pixel_LEN: the account of RGB pixels in serial
  * @retval None
  */
void RGB_WHITE(u16 Pixel_LEN)
{
    u8 i;
    for (i = 0; i < Pixel_LEN; i++)
        rgb_SetColor(i, WHITE);

    rgb_SendArray();
}

/* Complicated Color Effects -------------------------------------------------*/
RGBColor_TypeDef Colourful_Wheel(u8 WheelPos)
{
    WheelPos = 255 - WheelPos;
    RGBColor_TypeDef color;
    if (WheelPos < 85)
    {
        color.R = 255 - WheelPos * 3;
        color.G = 0;
        color.B = WheelPos * 3;
        return color;
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        color.R = 0;
        color.G = WheelPos * 3;
        color.B = 255 - WheelPos * 3;
        return color;
    }

    WheelPos -= 170;
    color.R = WheelPos * 3;
    color.G = 255 - WheelPos * 3;
    color.B = 0;

    return color;
}

void rainbowCycle(u8 Pixel_LEN)
{
    u16 i, j = 0, k = 256;
    for (j = 0; j < 1023; j++) // 1 cycles of all colors on wheel
    {
        for (i = 0; i < Pixel_LEN; i++)
            rgb_SetColor(i, Colourful_Wheel(((i * k / Pixel_LEN) + j) & 255)); //k值:RGB像素点之间的差距
        rgb_SendArray();
        delay_ms(10);
    }
}
