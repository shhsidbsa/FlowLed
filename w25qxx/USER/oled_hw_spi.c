#include "oled_hw_spi.h"
#include "oledfont.h"
#include "bmp1.h"
#include "delay.h"
#include "w25qxx.h"

u8 OLED_GRAM[128][8];
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��
void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //ʹ��GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //ʹ��SPI2ʱ��

    //GPIOFB3,4,5��ʼ������
    GPIO_InitStructure.GPIO_Pin = OLED_SCK_Pin | OLED_MOSI_Pin; //PB13��15���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           //100MHz
    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //����
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��

    GPIO_SetBits(GPIOB, OLED_SCK_Pin | OLED_MOSI_Pin);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2); //PB13����Ϊ SPI2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2); //PB15����Ϊ SPI2

    //����ֻ���SPI�ڳ�ʼ��
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);  //��λSPI2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE); //ֹͣ��λSPI2

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                        //����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                    //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                          //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                            //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                   //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //CRCֵ����Ķ���ʽ

    SPI_Init(SPI2, &SPI_InitStructure); //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
    SPI_Cmd(SPI2, ENABLE);              //ʹ��SPI����
}

void OLED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //ʹ��GPIOBʱ��

    GPIO_InitStructure.GPIO_Pin = OLED_DC_Pin | OLED_RST_Pin | OLED_CS_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50MHz
    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����

    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��GPIO
}
//SPI2�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256
//fAPB2ʱ��һ��Ϊ84Mhz��
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); //�ж���Ч��
    SPI2->CR1 &= 0XFFC7;                                            //λ3-5���㣬�������ò�����
    SPI2->CR1 |= SPI_BaudRatePrescaler;                             //����SPI2�ٶ�
    SPI_Cmd(SPI2, ENABLE);                                          //ʹ��SPI2
}
//--------------------------------------------------------------------------------------------------------
//	�� �� ��: spi_master_send_recv_byte
//	����˵��: SPI �շ�����
//	��    ��: 	spi_chl��SPIM ͨ��
//				send_byte�����͵�����
//	�� �� ֵ: ��
uint8_t spi_master_send_recv_byte(uint8_t spi_byte)
{
    uint8_t time = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
    {
        time++;
        if (time > 200)
        {
            return 0;
        }
    }
    SPI_I2S_SendData(SPI2, spi_byte); //ͨ������SPIx����һ������

    time = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
    {
        time++;
        if (time > 200)
        {
            return 0;
        }
    }
    return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����
}

static void oled_write_byte(uint8_t write_byte)
{
    //	hal_spi_send_bytes(1, &write_byte, 1);	//Ӳ����ʽSPI(�м�㺯����װ)
    spi_master_send_recv_byte(write_byte); //�ײ�Ӳ��SPI
}
static void oled_write_operate(uint8_t mode, uint8_t dat)
{
    OLED_CS_LOW;

    if (mode) //д������
    {
        OLED_DC_HIGH;
    }
    else //д������
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

//	�� �� ��: oled_set_pos
//	����˵��: oled ��ʾ��������
//	��    ��: x:������;y��������
//	�� �� ֵ: ��
//	��    ��: 2020-04-11
void oled_set_pos(uint8_t x, uint8_t y)
{
    oled_write_operate(OLED_COMM, 0xb0 + (y & 0x0f));        //����ҳ(page)��ַ
    oled_write_operate(OLED_COMM, ((x & 0xf0) >> 4) | 0x10); //�и���λ��ַ
    oled_write_operate(OLED_COMM, (x & 0x0f) | 0x00);        //�е���λ��ַ
}

//	�� �� ��: oled_dis_clear
//	����˵��: oled �����ʾ
//  ��    ע: д��00���
void oled_dis_clear(void)
{
    uint8_t page = 0;
    uint8_t i = 0;

    for (page = 0; page < 8; page++)
    {
        oled_write_operate(OLED_COMM, 0xb0 + page); //����ҳ��ַ(0--7)
        oled_write_operate(OLED_COMM, 0x00);        //������ʾλ�á��е͵�ַ
        oled_write_operate(OLED_COMM, 0x10);        //������ʾλ�á��иߵ�ַ

        for (i = 0; i < 128; i++)
        {
            oled_write_operate(OLED_DATA, 0x00); //0x00����
        }
    }
}
//	�� �� ��: oled_dis_set
//	����˵��: oled ��ʾ����������
//  ��    ע: д��ff����
void oled_dis_set(void)
{
    uint8_t page = 0;
    uint8_t i = 0;

    for (page = 0; page < 8; page++)
    {
        oled_write_operate(OLED_COMM, 0xb0 + page); //����ҳ��ַ(0--7)
        oled_write_operate(OLED_COMM, 0x00);        //������ʾλ�á��е͵�ַ
        oled_write_operate(OLED_COMM, 0x10);        //������ʾλ�á��иߵ�ַ

        for (i = 0; i < 128; i++)
        {
            oled_write_operate(OLED_DATA, 0xff);
        }
    }
}
//	�� �� ��: oled_dis_on
//	����˵��: oled ����ʾ
//  ��    ע: д��AFָ���
void oled_dis_on(void)
{
    oled_write_operate(OLED_COMM, 0x8d); //��DCDC
    oled_write_operate(OLED_COMM, 0x14); //��DCDC
    oled_write_operate(OLED_COMM, 0xaf); //��OLED
}

//	�� �� ��: oled_dis_off
//	����˵��: oled �ر���ʾ
//  ��    ע: д��AEָ��ر�
void oled_dis_off(void)
{
    oled_write_operate(OLED_COMM, 0x8d); //����DCDC
    oled_write_operate(OLED_COMM, 0x10); //�ر�DCDC
    oled_write_operate(OLED_COMM, 0xae); //�ر�OLED
}

//	�� �� ��: oled_dis_one_char
//	����˵��: oled ��ʾ�����ַ�
//	��    ��: x��������(��);y:������(��);str����ʾ�ַ�
//	�� �� ֵ: ��
//	��    ��: 2020-04-11
void oled_dis_one_char(uint8_t x, uint8_t y, uint8_t str)
{
    uint8_t i = 0;
    uint8_t ret = 0;
    //ret = str -32;
    ret = str - ' '; //�õ�ƫ�ƺ��ֵ,��ASCLL�����һ������.���ڶ�ά������������λ��

    if (x > (MAX_COLUMN - 1)) //�г������,��������һ��
    {
        x = 0;
        if (SIZE == 8)
        {
            y = y + 1; //���8�ŵ��ַ�
        }
        if (SIZE == 16)
        {
            y = y + 2; //���16�ŵ��ַ�
        }
    }
    if (SIZE == 16)
    {
        oled_set_pos(x, y);
        //16������ֳ�������д��
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

//	�� �� ��: oled_dis_str
//	����˵��: oled ��ʾ����ַ�
//	��    ��: x��������(��);y:������(��);str����ʾ�ַ���
//	�� �� ֵ: ��
//	��    ��: 2020-04-11
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

//	�� �� ��: oled_dis_chinese
//	����˵��: oled ��ʾ����
//	��    ��: x��������(��);y:������(��);no����ʾ����λ�ڱ���λ��
//	�� �� ֵ: ��
void oled_dis_chinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t i = 0;
    oled_set_pos(x, y);

    for (i = 0; i < 16; i++) //��������Ѱַ
    {
        oled_write_operate(OLED_DATA, TEST[2 * no][i]);
    }

    oled_set_pos(x, y + 1);
    for (i = 0; i < 16; i++)
    {
        oled_write_operate(OLED_DATA, TEST[2 * no + 1][i]);
    }
}

//	�� �� ��: oled_dis_picture
//	����˵��: oled ��ʾͼƬ
//	��    ��: x��������(��);y:������(��);no����ʾ����λ�ڱ���λ��
//	�� �� ֵ: ��
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
    for (y = y0; y < y1; y++) //ÿ��(0 - 7 page)
    {
        oled_set_pos(x0, y);
        for (x = x0; x < x1; x++) //0~128
        {
            oled_write_operate(OLED_DATA, bmp[i++]);
        }
    }
}

//	�� �� ��: oled_dis_logo
//	����˵��: oled ��ʾͼƬlogo
//	��    ��: ��
//	�� �� ֵ: ��
//	��    ��: 2020-04-11
//  ��    ע: ����Ļ��ʾ
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

//	�� �� ��: oled_pow
//	����˵��: ���� m��n�η�
//	��    ��: m������;n��ָ��
//	�� �� ֵ: ��
//	��    ��: 2020-04-11
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t ret = 1;

    while (n--)
    {
        ret *= m;
    }

    return ret;
}

//	�� �� ��: oled_dis_num
//	����˵��: oled ����
//	��    ��: x��������(��);y:������(��);num����ʾ����;len�����ֳ���;size_num�����ִ�С
//	�� �� ֵ: ��
//	��    ��: 2020-04-11
void oled_dis_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size_num)
{
    uint8_t i = 0;
    uint8_t temp = 0;
    uint8_t enshow = 0;

    for (i = 0; i < len; i++)
    {
        temp = (num / (oled_pow(10, len - i - 1))) % 10; //����ʾ������һλһλȡ����
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
    OLED_GPIO_Init(); //oled cs/dc/rst ����io��ʼ��

    // OLED_RST_HIGH;
    delay_ms(200);
    OLED_RST_LOW;
    delay_ms(200);
    OLED_RST_HIGH;

    oled_write_operate(OLED_COMM, 0xAE); //�ر�OLED

    oled_write_operate(OLED_COMM, 0x00); //�����е�λ��ַ
    oled_write_operate(OLED_COMM, 0x10); //�����и�λ��ַ

    oled_write_operate(OLED_COMM, 0x40); //������ʼ�е�ַ��ӳ��RAM��ʾ��ʼ�� (0x40~7F)

    oled_write_operate(OLED_COMM, 0x81); //�Աȶ�����
    oled_write_operate(OLED_COMM, 0xCF); //Set SEG Output Current Brightness

    oled_write_operate(OLED_COMM, 0xA1); //0xA1: ���ҷ��ã�  0xA0: ������ʾ��Ĭ��0xA0��
    oled_write_operate(OLED_COMM, 0xC8); //0xC8: ���·��ã�  0xC0: ������ʾ��Ĭ��0xC0��
    oled_write_operate(OLED_COMM, 0xA6); //������������ʾ (0xa6:����;a7:����)

    oled_write_operate(OLED_COMM, 0xA8); //��������·��(1 to 64)
    oled_write_operate(OLED_COMM, 0x3F); //--1/64 duty

    oled_write_operate(OLED_COMM, 0xD3); //-������ʾƫ��(0x00~0x3F)
    oled_write_operate(OLED_COMM, 0x00); //-not offset

    oled_write_operate(OLED_COMM, 0xD5); //--set display clock divide ratio/oscillator frequency
    oled_write_operate(OLED_COMM, 0x80); //--set divide ratio, Set Clock as 100 Frames/Sec

    oled_write_operate(OLED_COMM, 0xD9); //--set pre-charge period
    oled_write_operate(OLED_COMM, 0xF1); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

    oled_write_operate(OLED_COMM, 0xDA); //--set com pins hardware configuration
    oled_write_operate(OLED_COMM, 0x12);

    oled_write_operate(OLED_COMM, 0xDB); //--set vcomh
    oled_write_operate(OLED_COMM, 0x40); //Set VCOM Deselect Level

    oled_write_operate(OLED_COMM, 0x20); //���õ�ַģʽ(0x00/0x01/0x02)
    oled_write_operate(OLED_COMM, 0x02); //ҳ��ַģʽ

    oled_write_operate(OLED_COMM, 0x8D); //--set Charge Pump enable/disable
    oled_write_operate(OLED_COMM, 0x14); //Charge Pump enable

    oled_write_operate(OLED_COMM, 0xA4); //��ʾ����(��ʾ:A4;����ʾ:A5)
    oled_write_operate(OLED_COMM, 0xA6); //������������ʾ (0xa6:����;a7:����)
    oled_write_operate(OLED_COMM, 0xAF); //����ʾ

    OLED_Clear(); //��ʼ����

    // oled_dis_clear();
    // oled_set_pos(0, 0);
}
void OLED_Refresh_Gram(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        oled_write_operate(OLED_COMM, 0xb0 + i);
        oled_write_operate(OLED_COMM, 0x00); //������ʾλ�á��е͵�ַ
        oled_write_operate(OLED_COMM, 0x10); //������ʾλ�á��иߵ�ַ
        for (n = 0; n < 128; n++)
            oled_write_operate(OLED_DATA, OLED_GRAM[n][i]);
    }
}

/*************************************************************************/
/*��������: ����                                                          */
/*************************************************************************/
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
        for (n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0X00;
    OLED_Refresh_Gram(); //������ʾ
}

void OLED_DrawPoint1(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp = 0;
    if (x > 127 || y > 63)
        return; //������Χ��.
    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (t)
        OLED_GRAM[x][pos] |= temp;
    else
        OLED_GRAM[x][pos] &= ~temp;
}
/*************************************************************************/
/*��������: ��ָ��λ����ʾһ���ַ�,���������ַ�                             */
/*��ڲ�����                                                                                                                          */
/*                      x:0~12                                                     */
/*                      y:0~63                                                                      */
/*                      mode:0,������ʾ;1,������ʾ                                                       */
/*            size:ѡ������ 16/12                                        */
/*************************************************************************/
void OLED_ShowChar1(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    chr = chr - ' ';                                           //�õ�ƫ�ƺ��ֵ
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
            temp = asc2_1206[chr][t]; //����1206����
        else if (size == 16)
            temp = asc2_1608[chr][t]; //����1608����
        else if (size == 24)
            temp = asc2_2412[chr][t]; //����2412����
        else
            return; //û�е��ֿ�
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
//m^n����
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
        result *= m;
    return result;
}

/*************************************************************************/
/*��������: ��ʾ�ַ���												                             */
/*��ڲ����� 																														 */
/*						x,y:�������                                                */
/*						size:�����С                                	 	 				   */
/*						*p:�ַ�����ʼ��ַ	  								         	        		 */
/*************************************************************************/
void OLED_ShowString1(u8 x, u8 y, const u8 *p, u8 size)
{
    while ((*p <= '~') && (*p >= ' ')) //�ж��ǲ��ǷǷ��ַ�!
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
    if (cmd) //д������
    {
        OLED_DC_HIGH;
    }
    else //д������
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
//���Ժ���
void OLED_ColorTurn(u8 i)
{
    if (i == 0)
    {
        SPI_WriteByte(0xA6, OLED_COMM); //������ʾ
    }
    if (i == 1)
    {
        SPI_WriteByte(0xA7, OLED_COMM); //��ɫ��ʾ
    }
}

//��Ļ��ת180��
void OLED_DisplayTurn(u8 i)
{
    if (i == 0)
    {
        SPI_WriteByte(0xC8, OLED_COMM); //������ʾ
        SPI_WriteByte(0xA1, OLED_COMM);
    }
    if (i == 1)
    {
        SPI_WriteByte(0xC0, OLED_COMM); //��ת��ʾ
        SPI_WriteByte(0xA0, OLED_COMM);
    }
}
///////////////
//����
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

//���һ����
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

//����
//x:0~128
//y:0~64
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2)
{
    u8 i, k, k1, k2, y0;
    if ((x1 < 0) || (x2 > 128) || (y1 < 0) || (y2 > 64) || (x1 > x2) || (y1 > y2))
        return;
    if (x1 == x2) //������
    {
        for (i = 0; i < (y2 - y1); i++)
        {
            OLED_DrawPoint(x1, y1 + i);
        }
    }
    else if (y1 == y2) //������
    {
        for (i = 0; i < (x2 - x1); i++)
        {
            OLED_DrawPoint(x1 + i, y1);
        }
    }
    else //��б��
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
//x,y:Բ������
//r:Բ�İ뾶
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
        num = (a * a + b * b) - r * r; //���㻭�ĵ���Բ�ĵľ���
        if (num > 0)
        {
            b--;
            a--;
        }
    }
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size:ѡ������ 12/16/24
//ȡģ��ʽ ����ʽ
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1)
{
    u8 i, m, temp, size2, chr1;
    u8 y0 = y;
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2); //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    chr1 = chr - ' ';                                          //����ƫ�ƺ��ֵ
    for (i = 0; i < size2; i++)
    {
        if (size1 == 12)
        {
            temp = asc2_1206[chr1][i];
        } //����1206����
        else if (size1 == 16)
        {
            temp = asc2_1608[chr1][i];
        } //����1608����
        else if (size1 == 24)
        {
            temp = asc2_2412[chr1][i];
        } //����2412����
        else
            return;
        for (m = 0; m < 8; m++) //д������
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

//��ʾ�ַ���
//x,y:�������
//size1:�����С
//*chr:�ַ�����ʼ��ַ
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1)
{
    while ((*chr >= ' ') && (*chr <= '~')) //�ж��ǲ��ǷǷ��ַ�!
    {
        OLED_ShowChar(x, y, *chr, size1);
        x += size1 / 2;
        if (x > 128 - size1) //����
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

////��ʾ2������
////x,y :�������
////len :���ֵ�λ��
////size:�����С
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

//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//ȡģ��ʽ ����ʽ
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
            } //����16*16����
            else if (size1 == 24)
            {
                temp = Hzk2[chr1][i];
            } //����24*24����
            else if (size1 == 32)
            {
                temp = Hzk3[chr1][i];
            } //����32*32����
            else if (size1 == 64)
            {
                temp = Hzk4[chr1][i];
            } //����64*64����
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
* Description    : ȡGBK���� ����
* Input          : - *c: �����GBK���룬��'��' 
* Output         : - *pBuffer: ������ݵ�ָ��
* Return         : None
* Attention		 : ����һ��GBK���룬ȡ������32Byte��ʾ���벢�����ŵ�һ��32byte����ʾ����pBuffer[]��
*******************************************************************************/
void GetGBKCode(unsigned char *pBuffer, unsigned char *c)
{
    unsigned char High8bit, Low8bit;
    High8bit = *c;      /* ȡ��8λ���� */
    Low8bit = *(c + 1); /* ȡ��8λ���� */

    W25QXX_Read(pBuffer, ((High8bit - 0xb0) * 94 + Low8bit - 0xa1) * 64, 64);
    printf("\r\n %x \t %x \t %d", High8bit, Low8bit, ((High8bit - 0xb0) * 94 + Low8bit - 0xa1) * 64);
    printf("\r\n ��W25x16 SPI FLASH������������: %s", pBuffer);
    printf("\r\n");
}

void PutChinese(uint16_t Xpos, uint16_t Ypos, uint8_t *str, u8 size)
{
    uint8_t i = 0, j = 0, k = 0;
    uint8_t buffer[64], mbuffer[32];
    uint16_t tmp_char = 0;
    uint8_t temp;
    u8 x0 = Xpos, y0 = Ypos;
    GetGBKCode(buffer, str); /* ȡ��ģ���� */

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
    //             // LCD_SetPoint(Xpos + i, Ypos + j, Color); /* �ַ���ɫ */
    //             OLED_DrawPoint(Xpos + i, Ypos + j);
    //         else
    //             // LCD_SetPoint(Xpos + i, Ypos + j, bkColor); /* ������ɫ */
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

//num ��ʾ���ֵĸ���
//space ÿһ����ʾ�ļ��
void OLED_ScrollDisplay(u8 num, u8 space)
{
    u8 i, n, t = 0, m = 0, r;
    while (1)
    {
        if (m == 0)
        {
            OLED_ShowChinese(112, 24, t, 16); //д��һ�����ֱ�����OLED_GRAM[][]������
            t++;
        }
        if (t == num)
        {
            for (r = 0; r < 16 * space; r++) //��ʾ���
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
        for (i = 1; i < 128; i++) //ʵ������
        {
            for (n = 0; n < 8; n++)
            {
                OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
            }
        }
        OLED_Refresh_Gram();
    }
}

//����д�����ݵ���ʼλ��
void OLED_WR_BP(u8 x, u8 y)
{
    SPI_WriteByte(0xb0 + y, OLED_COMM); //��������ʼ��ַ
    SPI_WriteByte(((x & 0xf0) >> 4) | 0x10, OLED_COMM);
    SPI_WriteByte((x & 0x0f), OLED_COMM);
}

//x0,y0���������
//x1,y1���յ�����
//BMP[]��Ҫд���ͼƬ����
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
