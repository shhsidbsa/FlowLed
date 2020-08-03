#include "fontupdate.h"
#include "w25qxx.h"
#include "string.h"
#include "oled_hw_spi.h"
#include "delay.h"
#include "usart.h"

extern u16 Usart6_Rec_Len;
extern u32 GBK_OVER_Flag;
extern u8 GBK_BUF_Flag;
extern u8 Usart6_Rece_Buf0[Usart6_DMA_Len];
extern u8 Usart6_Rece_Buf1[Usart6_DMA_Len];

#define UNI_GBK 174344
#define GBK12_FON 574560
#define GBK16_FON 766080
#define GBK24_FON 1723608
//字库区域占用的总扇区数大小(3个字库+unigbk表+字库信息=3238700字节,约占791个W25QXX扇区)
// #define FONTSECSIZE 791
#define FONTSIZE 749012
#define FONTSECSIZE 185
//字库存放起始地址
#define FONTINFOADDR 1024 * 1024 * 0 //字库存放首地址
//定义各个字库的大小
#define UNIGBK 171 * 1024         //171KB
#define GBK12_FONSIZE 562 * 1024  //562KB
#define GBK16_FONSIZE 749 * 1024  //749KB
#define GBK24_FONSIZE 1684 * 1024 //1684KB

//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

//初始化字体
//返回值:0,字库完好.
//其他,字库丢失
u8 font_init()
{
    u8 t = 0;
    W25QXX_Init();
    while (t < 10) //连续读取10次,都是错误,说明确实是有问题,得更新字库了
    {
        t++;
        W25QXX_Read((u8 *)&ftinfo, FONTINFOADDR, sizeof(ftinfo)); //读出ftinfo结构体数据
        if (ftinfo.fontok == 0XAA)
            break;
        delay_ms(20);
    }
    if (ftinfo.fontok != 0XAA)
        return 1;
    return 0;
}

//显示当前字体更新进度
//x,y:坐标
//size:字体大小
//fsize:整个文件大小
//pos:当前文件指针位置
u32 fupd_prog(u16 x, u16 y, u8 size, u32 fsize, u32 pos)
{
    float prog;
    u8 t = 0XFF;
    prog = (float)pos / fsize;
    prog *= 100;
    if (t != prog)
    {
        // LCD_ShowString(x + 3 * size / 2, y, 240, 320, size, "%");
        t = prog;
        if (t > 100)
        {
            t = 100;
            printf("%d\r\n", t);
        }
        // LCD_ShowNum(x, y, t, 3, size); //显示数值
        OLED_ShowNum(x, y, t, 3, 12);
    }
    return 0;
}

//更新某一个
//x,y:坐标
//size:字体大小
//fx:更新的内容 0,ungbk;1,gbk12;2,gbk16;3,gbk24;
//返回值:0,成功;其他,失败.
u8 updata_fontx(u16 x, u16 y, u8 size, u8 fx)
{
    u32 flashaddr = 0;
    u8 res;
    u32 offx = 0;
    u32 fsize = 0;
    switch (fx)
    {
    case 0:                                              //更新UNIGBK.BIN
        ftinfo.ugbkaddr = FONTINFOADDR + sizeof(ftinfo); //信息头之后，紧跟UNIGBK转换码表
        fsize = ftinfo.ugbksize = UNIGBK;                //UNIGBK大小
        flashaddr = ftinfo.ugbkaddr;
        // printf("%d\t%d\t%d\r\n", fsize, flashaddr, sizeof(ftinfo));
        printf("Please send UNIGBK.bin\r\n");
        break;
    case 1:
        ftinfo.f12addr = ftinfo.ugbkaddr + ftinfo.ugbksize; //UNIGBK之后，紧跟GBK12字库
        fsize = ftinfo.gbk12size = GBK12_FONSIZE;           //GBK12字库大小
        flashaddr = ftinfo.f12addr;                         //GBK12的起始地址
        // printf("%d\t%d\r\n", fsize, flashaddr);
        printf("Please send GBK12.FON\r\n");
        break;
    case 2:
        ftinfo.f16addr = ftinfo.f12addr + ftinfo.gbk12size; //GBK12之后，紧跟GBK16字库
        fsize = ftinfo.gbk16size = GBK16_FONSIZE;           //GBK16字库大小
        flashaddr = ftinfo.f16addr;                         //GBK16的起始地址
        printf("Please send GBK16.FON\r\n");
        break;
    case 3:
        ftinfo.f24addr = ftinfo.f16addr + ftinfo.gbk16size; //GBK16之后，紧跟GBK24字库
        fsize = ftinfo.gkb24size = GBK24_FONSIZE;           //GBK24字库大小
        flashaddr = ftinfo.f24addr;                         //GBK24的起始地址
        printf("Please send GBK24.FON\r\n");
        break;
    }

    OLED_ShowString(114, y, "%", 12);

    fupd_prog(x, y, size, fsize, offx); //进度显示
    while (1)                           //死循环执行
    {
        // if (fx == 1)
        //     printf("fx==1\r\n");
        if (GBK_OVER_Flag)
            GBK_OVER_Flag++;
        if (GBK_BUF_Flag != 2)
        {
            GBK_OVER_Flag = 1;
            if (GBK_BUF_Flag == 0)
                W25QXX_Write(Usart6_Rece_Buf0, offx + flashaddr, Usart6_DMA_Len); //开始写入Usart6_DMA_Len个数据
            else if (GBK_BUF_Flag == 1)
                W25QXX_Write(Usart6_Rece_Buf1, offx + flashaddr, Usart6_DMA_Len); //开始写入Usart6_DMA_Len个数据
            offx += Usart6_DMA_Len;
            GBK_BUF_Flag = 2;
            fupd_prog(x, y, size, fsize, offx); //进度显示
        }
        delay_us(100);
        if (GBK_OVER_Flag > (WATE_TIME + 10) * 10) //超过正常时间10ms则说明此字库发送完毕
            break;
    }
    // printf("%d\r\n", offx);
    if (DMA_GetCurrentMemoryTarget(DMA2_Stream1) == 1)
        W25QXX_Write(Usart6_Rece_Buf1, offx + flashaddr, Usart6_DMA_Len - DMA_GetCurrDataCounter(DMA2_Stream1)); //将DMA最后的一帧数据写入FLASH
    else
        W25QXX_Write(Usart6_Rece_Buf0, offx + flashaddr, Usart6_DMA_Len - DMA_GetCurrDataCounter(DMA2_Stream1)); //将DMA最后的一帧数据写入FLASH
    fupd_prog(x, y, size, fsize, fsize);                                                                         //进度显示

    // printf("%d\r\n", Usart6_DMA_Len - DMA_GetCurrDataCounter(DMA2_Stream1));
    printf("This Font updated successfull!\r\n");
    uart6_init(BAUD_RATE); //重新初始化串口及DMA
    GBK_OVER_Flag = 0;
    GBK_BUF_Flag = 2;

    return res;
}

//更新字体文件,UNIGBK,GBK12,GBK16,GBK24一起更新
//x,y:提示信息的显示地址
//size:字体大小
//提示信息字体大小
//返回值:0,更新成功;
//		 其他,错误代码.
u8 update_font(u16 x, u16 y, u8 size)
{
    u16 i, j;

    // LCD_ShowString(x, y, 240, 320, size, (u8 *)"Erasing sectors... "); //提示正在擦除扇区
    OLED_ShowString(x, y, "Erasing sectors ", 12);
    OLED_ShowString(114, 12, "%", 12);

    for (i = 0; i < FONTSECSIZE; i++) //先擦除字库区域,提高写入速度
    {
        // fupd_prog(x + 20 * size / 2, y, size, FONTSECSIZE, i);                         //进度显示
        fupd_prog(96, y, size, FONTSECSIZE, i); //进度显示

        W25QXX_Read((u8 *)Usart6_Rece_Buf1, ((FONTINFOADDR / 4096) + i) * 4096, 4096); //读出整个扇区的内容(借用一下DMA缓冲区)
        for (j = 0; j < 4096; j++)                                                     //校验数据
        {
            if (Usart6_Rece_Buf1[j] != 0XFF)
                break; //需要擦除
        }
        if (j != 4096)
            W25QXX_Erase_Sector((FONTINFOADDR / 4096) + i); //需要擦除的扇区
    }
    fupd_prog(96, y, size, FONTSECSIZE, FONTSECSIZE); //进度显示

    delay_ms(100);

    // LCD_ShowString(x, y, 240, 320, size, (u8 *)"Updating UNIGBK.BIN  ");
    OLED_ShowString(x, y + 12, "Updating UNIGBK ", 12);
    updata_fontx(96, y + 12, size, 0); //更新UNIGBK.BIN

    // LCD_ShowString(x, y, 240, 320, size, (u8 *)"Updating GBK12.FON  ");
    OLED_ShowString(x, y + 24, "Updating GBK12  ", 12);
    updata_fontx(96, y + 24, size, 1); //更新GBK12.FON

    // LCD_ShowString(x, y, 240, 320, size, (u8 *)"Updating GBK16.FON  ");
    // OLED_ShowString(x, y + 12, "Updating GBK16.FON", 12);
    // updata_fontx(96, y + 12, size, 2); //更新GBK16.FON

    // LCD_ShowString(x, y, 240, 320, size, (u8 *)"Updating GBK24.FON  ");
    // OLED_ShowString(x, y + 12, "Updating GBK24.FON", 12);
    // updata_fontx(96, y + 12, size, 3); //更新GBK24.FON

    //全部更新好了
    ftinfo.fontok = 0XAA;
    W25QXX_Write((u8 *)&ftinfo, FONTINFOADDR, sizeof(ftinfo)); //保存字库信息
    printf("All Font file updated successfull!!!\r\n");
    // LCD_Clear(WHITE);

    return 0;
}
