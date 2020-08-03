#include "sys.h"
#include "fontupdate.h"
#include "w25qxx.h"
#include "oled_hw_spi.h"
#include "text.h"
#include "string.h"
#include "usart.h"

//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小
//size:字体大小
void Get_HzMat(unsigned char *code, unsigned char *mat, u8 size)
{
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); //得到字体一个字符对应点阵集所占的字节数
    qh = *code;
    ql = *(++code);
    if (qh < 0x81 || ql < 0x40 || ql == 0xff || qh == 0xff) //非 常用汉字
    {
        for (i = 0; i < csize; i++)
            *mat++ = 0x00; //填充满格
        return;            //结束访问
    }
    if (ql < 0x7f)
        ql -= 0x40; //注意!        //当 GBKL<0X7F 时： Hp=((GBKH-0x81)*190+GBKL-0X40)*(size*2)；
    else
        ql -= 0x41; //当 GBKL>0X80 时： Hp=((GBKH-0x81)*190+GBKL-0X41)*(size*2)；
    qh -= 0x81;
    foffset = ((unsigned long)190 * qh + ql) * csize; //得到字库中的字节偏移量
    switch (size)
    {
    case 12:
        // W25QXX_Read(mat, foffset + ftinfo.f12addr, csize);
        W25QXX_Read(mat, foffset + 175137, csize);
        break;
    case 16:
        W25QXX_Read(mat, foffset + ftinfo.f16addr, csize);
        break;
    case 24:
        W25QXX_Read(mat, foffset + ftinfo.f24addr, csize);
        break;
    }
}

//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小
//mode:0,正常显示,1,叠加显示
void Show_Font(u16 x, u16 y, u8 *font, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u16 y0 = y;
    u8 dzk[72];
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); //得到字体一个字符对应点阵集所占的字节数
    if (size != 12 && size != 16 && size != 24)
        return;                 //不支持的size
    Get_HzMat(font, dzk, size); //得到相应大小的点阵数据
    for (t = 0; t < csize; t++)
    {
        temp = dzk[t]; //得到点阵数据
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                OLED_DrawPoint(x, y);
            else if (mode == 0)
                OLED_ClearPoint(x, y);

            temp <<= 1;
            y++;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
    OLED_Refresh_Gram();
}
//在指定位置开始显示一个字符串
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式
// void Show_Str(u16 x, u16 y, u16 width, u16 height, u8 *str, u8 size, u8 mode)
// {
//     u16 x0 = x;
//     u16 y0 = y;
//     u8 bHz = 0;       //字符或者中文
//     while (*str != 0) //数据未结束
//     {
//         if (!bHz)
//         {
//             if (*str > 0x80)
//                 bHz = 1; //中文
//             else         //字符
//             {
//                 if (x > (x0 + width - size / 2)) //换行
//                 {
//                     y += size;
//                     x = x0;
//                 }
//                 if (y > (y0 + height - size))
//                     break;      //越界返回
//                 if (*str == 13) //换行符号
//                 {
//                     y += size;
//                     x = x0;
//                     str++;
//                 }
//                 else
//                     // LCD_ShowChar(x, y, *str, size, mode); //有效部分写入
//                     OLED_ShowString(x, y, *str, 12);
//                 str++;
//                 x += size / 2; //字符,为全字的一半
//             }
//         }
//         else //中文
//         {
//             bHz = 0;                     //有汉字库
//             if (x > (x0 + width - size)) //换行
//             {
//                 y += size;
//                 x = x0;
//             }
//             if (y > (y0 + height - size))
//                 break;                        //越界返回
//             Show_Font(x, y, str, size, mode); //显示这个汉字,空心显示
//             str += 2;
//             x += size; //下一个汉字偏移
//         }
//     }
// }

//显示字符串
//x,y:起点坐标
//size1:字体大小
//*chr:字符串起始地址
void Show_Str(u8 x, u8 y, u8 *chr, u8 size1)
{
    u8 bHz = 0; //字符或者中文

    // while ((*chr >= ' ') && (*chr <= '~')) //判断是不是非法字符!
    while (*chr != 0)
    {
        if (!bHz)
        {
            if (*chr > 0x80)
                bHz = 1; //中文
            else         //字符
            {
                OLED_ShowChar(x, y, *chr, size1);
                x += size1 / 2;
                if (x > 128 - size1) //换行
                {
                    x = 0;
                    y += 12;
                }
                chr++;
            }
        }
        else //中文
        {
            bHz = 0;                     //有汉字库
            Show_Font(x, y, chr, 12, 1); //显示这个汉字,空心显示
            x += size1 / 2;
            if (x > 128 - size1) //换行
            {
                x = 0;
                y += 12;
            }
            chr += 2;
            x += 6; //下一个汉字偏移
        }
    }
    // OLED_Refresh_Gram();
}

//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度
// void Show_Str_Mid(u16 x, u16 y, u8 *str, u8 size, u8 len)
// {
//     u16 strlenth = 0;
//     strlenth = strlen((const char *)str);
//     strlenth *= size / 2;
//     if (strlenth > len)
//         Show_Str(x, y, lcddev.width, lcddev.height, str, size, 1);
//     else
//     {
//         strlenth = (len - strlenth) / 2;
//         Show_Str(strlenth + x, y, lcddev.width, lcddev.height, str, size, 1);
//     }
// }
