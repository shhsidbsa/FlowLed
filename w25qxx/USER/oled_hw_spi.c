#include "oled_hw_spi.h"
#include "oledfont.h"
#include "bmp1.h"
#include "delay.h"
#include "w25qxx.h"

u8 OLED_GRAM[128][8];
//以下是SPI模块的初始化代码，配置成主机模式
//SPI口初始化
//这里针是对SPI2的初始化
void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //使能SPI2时钟

    //GPIOFB3,4,5初始化设置
    GPIO_InitStructure.GPIO_Pin = OLED_SCK_Pin | OLED_MOSI_Pin; //PB13，15复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           //100MHz
    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化

    GPIO_SetBits(GPIOB, OLED_SCK_Pin | OLED_MOSI_Pin);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2); //PB13复用为 SPI2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2); //PB15复用为 SPI2

    //这里只针对SPI口初始化
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);  //复位SPI2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE); //停止复位SPI2

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                        //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                    //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                          //串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                            //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                   //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //CRC值计算的多项式

    SPI_Init(SPI2, &SPI_InitStructure); //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
    SPI_Cmd(SPI2, ENABLE);              //使能SPI外设
}

void OLED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟

    GPIO_InitStructure.GPIO_Pin = OLED_DC_Pin | OLED_RST_Pin | OLED_CS_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50MHz
    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉

    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIO
}
//SPI2速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256
//fAPB2时钟一般为84Mhz：
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); //判断有效性
    SPI2->CR1 &= 0XFFC7;                                            //位3-5清零，用来设置波特率
    SPI2->CR1 |= SPI_BaudRatePrescaler;                             //设置SPI2速度
    SPI_Cmd(SPI2, ENABLE);                                          //使能SPI2
}
//--------------------------------------------------------------------------------------------------------
//	函 数 名: spi_master_send_recv_byte
//	功能说明: SPI 收发数据
//	形    参: 	spi_chl：SPIM 通道
//				send_byte：发送的数据
//	返 回 值: 无
uint8_t spi_master_send_recv_byte(uint8_t spi_byte)
{
    uint8_t time = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        time++;
        if (time > 200)
        {
            return 0;
        }
    }
    SPI_I2S_SendData(SPI2, spi_byte); //通过外设SPIx发送一个数据

    time = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
    {
        time++;
        if (time > 200)
        {
            return 0;
        }
    }
    return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据
}

static void oled_write_byte(uint8_t write_byte)
{
    //	hal_spi_send_bytes(1, &write_byte, 1);	//硬件方式SPI(中间层函数封装)
    spi_master_send_recv_byte(write_byte); //底层硬件SPI
}
static void oled_write_operate(uint8_t mode, uint8_t dat)
{
    OLED_CS_LOW;

    if (mode) //写入数据
    {
        OLED_DC_HIGH;
    }
    else //写入命令
    {
        OLED_DC_LOW;
    }

    // oled_write_byte(dat);
    // OLED_CS_HIGH;
    SPI_I2S_SendData(SPI2, dat);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
        ;
    OLED_DC_LOW;
}

//	函 数 名: oled_set_pos
//	功能说明: oled 显示坐标设置
//	形    参: x:横坐标;y：纵坐标
//	返 回 值: 无
//	日    期: 2020-04-11
void oled_set_pos(uint8_t x, uint8_t y)
{
    oled_write_operate(OLED_COMM, 0xb0 + (y & 0x0f));        //设置页(page)地址
    oled_write_operate(OLED_COMM, ((x & 0xf0) >> 4) | 0x10); //列高四位地址
    oled_write_operate(OLED_COMM, (x & 0x0f) | 0x00);        //列低四位地址
}

//	函 数 名: oled_dis_clear
//	功能说明: oled 清除显示
//  备    注: 写入00清除
void oled_dis_clear(void)
{
    uint8_t page = 0;
    uint8_t i = 0;

    for (page = 0; page < 8; page++)
    {
        oled_write_operate(OLED_COMM, 0xb0 + page); //设置页地址(0--7)
        oled_write_operate(OLED_COMM, 0x00);        //设置显示位置—列低地址
        oled_write_operate(OLED_COMM, 0x10);        //设置显示位置—列高地址

        for (i = 0; i < 128; i++)
        {
            oled_write_operate(OLED_DATA, 0x00); //0x00清屏
        }
    }
}
//	函 数 名: oled_dis_set
//	功能说明: oled 显示出可视区域
//  备    注: 写入ff设置
void oled_dis_set(void)
{
    uint8_t page = 0;
    uint8_t i = 0;

    for (page = 0; page < 8; page++)
    {
        oled_write_operate(OLED_COMM, 0xb0 + page); //设置页地址(0--7)
        oled_write_operate(OLED_COMM, 0x00);        //设置显示位置—列低地址
        oled_write_operate(OLED_COMM, 0x10);        //设置显示位置—列高地址

        for (i = 0; i < 128; i++)
        {
            oled_write_operate(OLED_DATA, 0xff);
        }
    }
}
//	函 数 名: oled_dis_on
//	功能说明: oled 打开显示
//  备    注: 写入AF指令打开
void oled_dis_on(void)
{
    oled_write_operate(OLED_COMM, 0x8d); //打开DCDC
    oled_write_operate(OLED_COMM, 0x14); //打开DCDC
    oled_write_operate(OLED_COMM, 0xaf); //打开OLED
}

//	函 数 名: oled_dis_off
//	功能说明: oled 关闭显示
//  备    注: 写入AE指令关闭
void oled_dis_off(void)
{
    oled_write_operate(OLED_COMM, 0x8d); //设置DCDC
    oled_write_operate(OLED_COMM, 0x10); //关闭DCDC
    oled_write_operate(OLED_COMM, 0xae); //关闭OLED
}

//	函 数 名: oled_dis_one_char
//	功能说明: oled 显示单个字符
//	形    参: x：横坐标(列);y:纵坐标(行);str：显示字符
//	返 回 值: 无
//	日    期: 2020-04-11
void oled_dis_one_char(uint8_t x, uint8_t y, uint8_t str)
{
    uint8_t i = 0;
    uint8_t ret = 0;
    //ret = str -32;
    ret = str - ' '; //得到偏移后的值,对ASCLL码进行一个减法.即在二维数组里找它的位置

    if (x > (MAX_COLUMN - 1)) //列超过最大,更换到下一行
    {
        x = 0;
        if (SIZE == 8)
        {
            y = y + 1; //针对8号的字符
        }
        if (SIZE == 16)
        {
            y = y + 2; //针对16号的字符
        }
    }
    if (SIZE == 16)
    {
        oled_set_pos(x, y);
        //16的字体分成两部分写入
        for (i = 0; i < 8; i++)
        {
            oled_write_operate(OLED_DATA, F8X16[ret * 16 + i]);
        }
        oled_set_pos(x, y + 1);
        for (i = 0; i < 8; i++)
        {
            oled_write_operate(OLED_DATA, F8X16[ret * 16 + i + 8]);
        }
    }
    else
    {
        oled_set_pos(x, y + 1);
        for (i = 0; i < 6; i++)
        {
            oled_write_operate(OLED_DATA, F6x8[ret][i]);
        }
    }
}

//	函 数 名: oled_dis_str
//	功能说明: oled 显示多个字符
//	形    参: x：横坐标(列);y:纵坐标(行);str：显示字符串
//	返 回 值: 无
//	日    期: 2020-04-11
void oled_dis_str(uint8_t x, uint8_t y, uint8_t *str)
{
    uint8_t i = 0;

    while (str[i] != '\0')
    {
        oled_dis_one_char(x, y, str[i]);
        x += 8;

        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        i++;
    }
}

//	函 数 名: oled_dis_chinese
//	功能说明: oled 显示汉字
//	形    参: x：横坐标(列);y:纵坐标(行);no：显示汉字位于编码位置
//	返 回 值: 无
void oled_dis_chinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t i = 0;
    oled_set_pos(x, y);

    for (i = 0; i < 16; i++) //数组行列寻址
    {
        oled_write_operate(OLED_DATA, TEST[2 * no][i]);
    }

    oled_set_pos(x, y + 1);
    for (i = 0; i < 16; i++)
    {
        oled_write_operate(OLED_DATA, TEST[2 * no + 1][i]);
    }
}

//	函 数 名: oled_dis_picture
//	功能说明: oled 显示图片
//	形    参: x：横坐标(列);y:纵坐标(行);no：显示汉字位于编码位置
//	返 回 值: 无
void oled_dis_picture(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *bmp)
{
    uint8_t x = 0;
    uint8_t y = 0;
    uint32_t i = 0;

    if ((y1 % 8) == 0)
    {
        y = y1 / 8;
    }
    else
    {
        y = y1 / 8 + 1;
    }
    for (y = y0; y < y1; y++) //每行(0 - 7 page)
    {
        oled_set_pos(x0, y);
        for (x = x0; x < x1; x++) //0~128
        {
            oled_write_operate(OLED_DATA, bmp[i++]);
        }
    }
}

//	函 数 名: oled_dis_logo
//	功能说明: oled 显示图片logo
//	形    参: 无
//	返 回 值: 无
//	日    期: 2020-04-11
//  备    注: 整屏幕显示
void oled_dis_logo(void)
{
    const uint8_t *pdata = myBitmap;
    uint8_t page = 0;
    uint8_t seg = 0;

    for (page = 0xb0; page < 0xb8; page++)
    {
        oled_write_operate(OLED_COMM, page);
        oled_write_operate(OLED_COMM, 0x10);
        oled_write_operate(OLED_COMM, 0x00);

        for (seg = 0; seg < 128; seg++)
        {
            oled_write_operate(OLED_DATA, *pdata++);
        }
    }
}

//	函 数 名: oled_pow
//	功能说明: 计算 m的n次方
//	形    参: m：底数;n：指数
//	返 回 值: 无
//	日    期: 2020-04-11
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t ret = 1;

    while (n--)
    {
        ret *= m;
    }

    return ret;
}

//	函 数 名: oled_dis_num
//	功能说明: oled 数字
//	形    参: x：横坐标(列);y:纵坐标(行);num：显示数字;len：数字长度;size_num：数字大小
//	返 回 值: 无
//	日    期: 2020-04-11
void oled_dis_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size_num)
{
    uint8_t i = 0;
    uint8_t temp = 0;
    uint8_t enshow = 0;

    for (i = 0; i < len; i++)
    {
        temp = (num / (oled_pow(10, len - i - 1))) % 10; //把显示的数字一位一位取出来
        if ((enshow == 0) && (i < (len - 1)))
        {
            if (temp == 0)
            {
                oled_dis_one_char(x + (size_num / 2) * i, y, ' ');
                continue;
            }
            else
            {
                enshow = 1;
            }
        }
        oled_dis_one_char(x + (size_num / 2) * i, y, temp + '0');
    }
}

void oled_init(void)
{
    OLED_GPIO_Init(); //oled cs/dc/rst 三个io初始化

    // OLED_RST_HIGH;
    delay_ms(200);
    OLED_RST_LOW;
    delay_ms(200);
    OLED_RST_HIGH;

    oled_write_operate(OLED_COMM, 0xAE); //关闭OLED

    oled_write_operate(OLED_COMM, 0x00); //设置列低位地址
    oled_write_operate(OLED_COMM, 0x10); //设置列高位地址

    oled_write_operate(OLED_COMM, 0x40); //设置起始行地址及映射RAM显示起始行 (0x40~7F)

    oled_write_operate(OLED_COMM, 0x81); //对比度设置
    oled_write_operate(OLED_COMM, 0xCF); //Set SEG Output Current Brightness

    oled_write_operate(OLED_COMM, 0xA1); //0xA1: 左右反置，  0xA0: 正常显示（默认0xA0）
    oled_write_operate(OLED_COMM, 0xC8); //0xC8: 上下反置，  0xC0: 正常显示（默认0xC0）
    oled_write_operate(OLED_COMM, 0xA6); //背景正反向显示 (0xa6:正显;a7:反显)

    oled_write_operate(OLED_COMM, 0xA8); //设置驱动路数(1 to 64)
    oled_write_operate(OLED_COMM, 0x3F); //--1/64 duty

    oled_write_operate(OLED_COMM, 0xD3); //-设置显示偏移(0x00~0x3F)
    oled_write_operate(OLED_COMM, 0x00); //-not offset

    oled_write_operate(OLED_COMM, 0xD5); //--set display clock divide ratio/oscillator frequency
    oled_write_operate(OLED_COMM, 0x80); //--set divide ratio, Set Clock as 100 Frames/Sec

    oled_write_operate(OLED_COMM, 0xD9); //--set pre-charge period
    oled_write_operate(OLED_COMM, 0xF1); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

    oled_write_operate(OLED_COMM, 0xDA); //--set com pins hardware configuration
    oled_write_operate(OLED_COMM, 0x12);

    oled_write_operate(OLED_COMM, 0xDB); //--set vcomh
    oled_write_operate(OLED_COMM, 0x40); //Set VCOM Deselect Level

    oled_write_operate(OLED_COMM, 0x20); //设置地址模式(0x00/0x01/0x02)
    oled_write_operate(OLED_COMM, 0x02); //页地址模式

    oled_write_operate(OLED_COMM, 0x8D); //--set Charge Pump enable/disable
    oled_write_operate(OLED_COMM, 0x14); //Charge Pump enable

    oled_write_operate(OLED_COMM, 0xA4); //显示开启(显示:A4;无显示:A5)
    oled_write_operate(OLED_COMM, 0xA6); //背景正反向显示 (0xa6:正显;a7:反显)
    oled_write_operate(OLED_COMM, 0xAF); //打开显示

    OLED_Clear(); //初始清屏

    // oled_dis_clear();
    // oled_set_pos(0, 0);
}
void OLED_Refresh_Gram(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        oled_write_operate(OLED_COMM, 0xb0 + i);
        oled_write_operate(OLED_COMM, 0x00); //设置显示位置—列低地址
        oled_write_operate(OLED_COMM, 0x10); //设置显示位置—列高地址
        for (n = 0; n < 128; n++)
            oled_write_operate(OLED_DATA, OLED_GRAM[n][i]);
    }
}

/*************************************************************************/
/*函数功能: 清屏                                                          */
/*************************************************************************/
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
        for (n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0X00;
    OLED_Refresh_Gram(); //更新显示
}

void OLED_DrawPoint1(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp = 0;
    if (x > 127 || y > 63)
        return; //超出范围了.
    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (t)
        OLED_GRAM[x][pos] |= temp;
    else
        OLED_GRAM[x][pos] &= ~temp;
}
/*************************************************************************/
/*函数功能: 在指定位置显示一个字符,包括部分字符                             */
/*入口参数：                                                                                                                          */
/*                      x:0~12                                                     */
/*                      y:0~63                                                                      */
/*                      mode:0,反白显示;1,正常显示                                                       */
/*            size:选择字体 16/12                                        */
/*************************************************************************/
void OLED_ShowChar1(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); //得到字体一个字符对应点阵集所占的字节数
    chr = chr - ' ';                                           //得到偏移后的值
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
            temp = asc2_1206[chr][t]; //调用1206字体
        else if (size == 16)
            temp = asc2_1608[chr][t]; //调用1608字体
        else if (size == 24)
            temp = asc2_2412[chr][t]; //调用2412字体
        else
            return; //没有的字库
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                OLED_DrawPoint1(x, y, mode);
            else
                OLED_DrawPoint1(x, y, !mode);
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
}
//m^n函数
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
        result *= m;
    return result;
}

/*************************************************************************/
/*函数功能: 显示字符串												                             */
/*入口参数： 																														 */
/*						x,y:起点坐标                                                */
/*						size:字体大小                                	 	 				   */
/*						*p:字符串起始地址	  								         	        		 */
/*************************************************************************/
void OLED_ShowString1(u8 x, u8 y, const u8 *p, u8 size)
{
    while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if (x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }
        if (y > (64 - size))
        {
            y = x = 0;
            OLED_Clear();
        }
        OLED_ShowChar1(x, y, *p, size, 1);
        x += size / 2;
        p++;
    }
}

void SPI_WriteByte(unsigned char data, unsigned char cmd)
{
    if (cmd) //写入数据
    {
        OLED_DC_HIGH;
    }
    else //写入命令
    {
        OLED_DC_LOW;
    }
    OLED_WB(data);
}

void OLED_WB(uint8_t data)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
        ;
    /* Send byte through the SPI2 peripheral */
    SPI_I2S_SendData(SPI2, data);
}

/********************/
/////////////////////
/////////////////////
/////////////////////
/////////////////////
/////////////////////
/////////////////////
/////////////////////
//反显函数
void OLED_ColorTurn(u8 i)
{
    if (i == 0)
    {
        SPI_WriteByte(0xA6, OLED_COMM); //正常显示
    }
    if (i == 1)
    {
        SPI_WriteByte(0xA7, OLED_COMM); //反色显示
    }
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
    if (i == 0)
    {
        SPI_WriteByte(0xC8, OLED_COMM); //正常显示
        SPI_WriteByte(0xA1, OLED_COMM);
    }
    if (i == 1)
    {
        SPI_WriteByte(0xC0, OLED_COMM); //反转显示
        SPI_WriteByte(0xA0, OLED_COMM);
    }
}
///////////////
//画点
//x:0~127
//y:0~63
void OLED_DrawPoint(u8 x, u8 y)
{
    u8 i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    OLED_GRAM[x][i] |= n;
}

//清除一个点
//x:0~127
//y:0~63
void OLED_ClearPoint(u8 x, u8 y)
{
    u8 i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
    OLED_GRAM[x][i] |= n;
    OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
}

//画线
//x:0~128
//y:0~64
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2)
{
    u8 i, k, k1, k2, y0;
    if ((x1 < 0) || (x2 > 128) || (y1 < 0) || (y2 > 64) || (x1 > x2) || (y1 > y2))
        return;
    if (x1 == x2) //画竖线
    {
        for (i = 0; i < (y2 - y1); i++)
        {
            OLED_DrawPoint(x1, y1 + i);
        }
    }
    else if (y1 == y2) //画横线
    {
        for (i = 0; i < (x2 - x1); i++)
        {
            OLED_DrawPoint(x1 + i, y1);
        }
    }
    else //画斜线
    {
        k1 = y2 - y1;
        k2 = x2 - x1;
        k = k1 * 10 / k2;
        for (i = 0; i < (x2 - x1); i++)
        {
            OLED_DrawPoint(x1 + i, y1 + i * k / 10);
        }
    }
}
//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x, u8 y, u8 r)
{
    int a, b, num;
    a = 0;
    b = r;
    while (2 * b * b >= r * r)
    {
        OLED_DrawPoint(x + a, y - b);
        OLED_DrawPoint(x - a, y - b);
        OLED_DrawPoint(x - a, y + b);
        OLED_DrawPoint(x + a, y + b);

        OLED_DrawPoint(x + b, y + a);
        OLED_DrawPoint(x + b, y - a);
        OLED_DrawPoint(x - b, y - a);
        OLED_DrawPoint(x - b, y + a);

        a++;
        num = (a * a + b * b) - r * r; //计算画的点离圆心的距离
        if (num > 0)
        {
            b--;
            a--;
        }
    }
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size:选择字体 12/16/24
//取模方式 逐列式
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1)
{
    u8 i, m, temp, size2, chr1;
    u8 y0 = y;
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2); //得到字体一个字符对应点阵集所占的字节数
    chr1 = chr - ' ';                                          //计算偏移后的值
    for (i = 0; i < size2; i++)
    {
        if (size1 == 12)
        {
            temp = asc2_1206[chr1][i];
        } //调用1206字体
        else if (size1 == 16)
        {
            temp = asc2_1608[chr1][i];
        } //调用1608字体
        else if (size1 == 24)
        {
            temp = asc2_2412[chr1][i];
        } //调用2412字体
        else
            return;
        for (m = 0; m < 8; m++) //写入数据
        {
            if (temp & 0x80)
                OLED_DrawPoint(x, y);
            else
                OLED_ClearPoint(x, y);
            temp <<= 1;
            y++;
            if ((y - y0) == size1)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

//显示字符串
//x,y:起点坐标
//size1:字体大小
//*chr:字符串起始地址
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1)
{
    while ((*chr >= ' ') && (*chr <= '~')) //判断是不是非法字符!
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
    OLED_Refresh_Gram();
}

//m^n
u32 OLED_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}

////显示2个数字
////x,y :起点坐标
////len :数字的位数
////size:字体大小
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1)
{
    u8 t, temp;
    for (t = 0; t < len; t++)
    {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        if (temp == 0)
        {
            OLED_ShowChar(x + (size1 / 2) * t, y, '0', size1);
        }
        else
        {
            OLED_ShowChar(x + (size1 / 2) * t, y, temp + '0', size1);
        }
    }
    OLED_Refresh_Gram();
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//取模方式 列行式
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1)
{
    u8 i, m, n = 0, temp, chr1;
    u8 x0 = x, y0 = y;
    u8 size3 = size1 / 8;
    while (size3--)
    {
        chr1 = num * size1 / 8 + n;
        n++;
        for (i = 0; i < size1; i++)
        {
            if (size1 == 16)
            {
                temp = Hzk1[chr1][i];
            } //调用16*16字体
            else if (size1 == 24)
            {
                temp = Hzk2[chr1][i];
            } //调用24*24字体
            else if (size1 == 32)
            {
                temp = Hzk3[chr1][i];
            } //调用32*32字体
            else if (size1 == 64)
            {
                temp = Hzk4[chr1][i];
            } //调用64*64字体
            else
                return;

            for (m = 0; m < 8; m++)
            {
                if (temp & 0x01)
                    OLED_DrawPoint(x, y);
                else
                    OLED_ClearPoint(x, y);
                temp >>= 1;
                y++;
            }
            x++;
            if ((x - x0) == size1)
            {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
}

/*******************************************************************************
* Function Name  : GetGBKCode
* Description    : 取GBK内码 数据
* Input          : - *c: 输入的GBK内码，如'我' 
* Output         : - *pBuffer: 存放数据的指针
* Return         : None
* Attention		 : 输入一个GBK内码，取得它的32Byte显示代码并将其存放到一个32byte的显示缓冲pBuffer[]中
*******************************************************************************/
void GetGBKCode(unsigned char *pBuffer, unsigned char *c)
{
    unsigned char High8bit, Low8bit;
    High8bit = *c;      /* 取高8位数据 */
    Low8bit = *(c + 1); /* 取低8位数据 */

    W25QXX_Read(pBuffer, ((High8bit - 0xb0) * 94 + Low8bit - 0xa1) * 64, 64);
    printf("\r\n %x \t %x \t %d", High8bit, Low8bit, ((High8bit - 0xb0) * 94 + Low8bit - 0xa1) * 64);
    printf("\r\n 从W25x16 SPI FLASH读出的数据是: %s", pBuffer);
    printf("\r\n");
}

void PutChinese(uint16_t Xpos, uint16_t Ypos, uint8_t *str, u8 size)
{
    uint8_t i = 0, j = 0, k = 0;
    uint8_t buffer[64], mbuffer[32];
    uint16_t tmp_char = 0;
    uint8_t temp;
    u8 x0 = Xpos, y0 = Ypos;
    GetGBKCode(buffer, str); /* 取字模数据 */

    for (k = 0; k < 64; k++)
    {
        if (buffer[k] < 0x40)
            buffer[k] -= 0x30;
        else
        {
            buffer[k] -= 0x57;
        }
    }
    for (k = 0; k < 32; k++)
    {
        // for (i = 0; i < size; i++)
        // {
        temp = buffer[2 * k] << 4;
        temp |= buffer[2 * k + 1];
        for (j = 0; j < 8; j++)
        {
            if (temp & 0x01)
                OLED_DrawPoint(Xpos, Ypos);
            else
                OLED_ClearPoint(Xpos, Ypos);
            temp >>= 1;
            Ypos++;
        }
        Xpos++;
        if ((Xpos - x0) == size)
        {
            Xpos = x0;
            y0 = y0 + 8;
        }
        Ypos = y0;
        // }
    }
    // for (k = 0; k < 32; k++)
    // {
    //     mbuffer[k] = buffer[2 * k];
    //     mbuffer[k] = ((mbuffer[k] & 0x0f) << 4);
    //     mbuffer[k] |= (buffer[2 * k + 1] & 0x0f);
    // }

    // for (i = 0; i < 16; i++)
    // {
    //     tmp_char = mbuffer[i * 2];
    //     tmp_char = (tmp_char << 8);
    //     tmp_char |= mbuffer[2 * i + 1];
    //     for (j = 0; j < 16; j++)
    //     {
    //         if ((tmp_char >> 15 - j) & 0x01 == 0x01)
    //             // LCD_SetPoint(Xpos + i, Ypos + j, Color); /* 字符颜色 */
    //             OLED_DrawPoint(Xpos + i, Ypos + j);
    //         else
    //             // LCD_SetPoint(Xpos + i, Ypos + j, bkColor); /* 背景颜色 */
    //             OLED_DrawPoint(Xpos + i, Ypos + j);
    //     }
    // }
}

void GUI_Chinese(uint16_t Xpos, uint16_t Ypos, uint8_t *str, u8 size1)
{
    do
    {
        PutChinese(Xpos, Ypos, str++, 16);
        str++;
        if (Xpos < 112)
        {
            // Ypos = 0;
            Xpos += 16;
        }
        else if (Ypos < 48)
        {
            Ypos += 16;
            Xpos = 0;
        }
        else
        {
            Xpos = 0;
            Ypos = 0;
        }
    } while (*str != 0);
    OLED_Refresh_Gram();
}

//num 显示汉字的个数
//space 每一遍显示的间隔
void OLED_ScrollDisplay(u8 num, u8 space)
{
    u8 i, n, t = 0, m = 0, r;
    while (1)
    {
        if (m == 0)
        {
            OLED_ShowChinese(112, 24, t, 16); //写入一个汉字保存在OLED_GRAM[][]数组中
            t++;
        }
        if (t == num)
        {
            for (r = 0; r < 16 * space; r++) //显示间隔
            {
                for (i = 1; i < 128; i++)
                {
                    for (n = 0; n < 8; n++)
                    {
                        OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
                    }
                }
                OLED_Refresh_Gram();
            }
            t = 0;
        }
        m++;
        if (m == 16)
        {
            m = 0;
        }
        for (i = 1; i < 128; i++) //实现左移
        {
            for (n = 0; n < 8; n++)
            {
                OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
            }
        }
        OLED_Refresh_Gram();
    }
}

//配置写入数据的起始位置
void OLED_WR_BP(u8 x, u8 y)
{
    SPI_WriteByte(0xb0 + y, OLED_COMM); //设置行起始地址
    SPI_WriteByte(((x & 0xf0) >> 4) | 0x10, OLED_COMM);
    SPI_WriteByte((x & 0x0f), OLED_COMM);
}

//x0,y0：起点坐标
//x1,y1：终点坐标
//BMP[]：要写入的图片数组
void OLED_ShowPicture(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[])
{
    u32 j = 0;
    u8 x = 0, y = 0;
    if (y % 8 == 0)
        y = 0;
    else
        y += 1;
    for (y = y0; y < y1; y++)
    {
        OLED_WR_BP(x0, y);
        for (x = x0; x < x1; x++)
        {
            SPI_WriteByte(BMP[j], OLED_DATA);
            j++;
        }
    }
}
