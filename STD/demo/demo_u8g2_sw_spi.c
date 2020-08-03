//FAILED
#include "sys.h"
#include "u8g2.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "u8g2_sw_spi.h"

static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
    //Initialize SPI peripheral
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        /* HAL initialization contains all what we need so we can skip this part. */
        break;

    //Function which implements a delay, arg_int contains the amount of ms
    case U8X8_MSG_DELAY_MILLI:
        delay_ms(arg_int);
        break;

    //Function which delays 10us
    case U8X8_MSG_DELAY_10MICRO:
        delay_us(10);
        break;

    //Function which delays 100ns
    case U8X8_MSG_DELAY_100NANO:
        // __NOP();
        // __NOP();
        {
            uint8_t delay_cnt = 0;
            while (delay_cnt++ < 10)
                ;
        }
        break;

    //Function to define the logic level of the clockline
    case U8X8_MSG_GPIO_SPI_CLOCK:
        if (arg_int)
            GPIO_SetBits(OLED_SCK_GPIO_Port, OLED_SCK_Pin);
        else
            GPIO_ResetBits(OLED_SCK_GPIO_Port, OLED_SCK_Pin);

        break;

    //Function to define the logic level of the data line to the display
    case U8X8_MSG_GPIO_SPI_DATA:
        if (arg_int)
            GPIO_SetBits(OLED_MOSI_GPIO_Port, OLED_MOSI_Pin);
        else
            GPIO_ResetBits(OLED_MOSI_GPIO_Port, OLED_MOSI_Pin);
        break;

    // Function to define the logic level of the OLED_CS_Pin line
    case U8X8_MSG_GPIO_CS:
        if (arg_int)
            GPIO_SetBits(OLED_CS_GPIO_Port, OLED_CS_Pin);
        else
            GPIO_ResetBits(OLED_CS_GPIO_Port, OLED_CS_Pin);
        break;

    //Function to define the logic level of the Data/ Command line
    case U8X8_MSG_GPIO_DC:
        if (arg_int)
            GPIO_SetBits(OLED_DC_GPIO_Port, OLED_DC_Pin);
        else
            GPIO_ResetBits(OLED_DC_GPIO_Port, OLED_DC_Pin);
        break;

    //Function to define the logic level of the Bit_RESET line
    case U8X8_MSG_GPIO_RESET:
        if (arg_int)
            GPIO_SetBits(OLED_RST_GPIO_Port, OLED_RST_Pin);
        else
            GPIO_ResetBits(OLED_RST_GPIO_Port, OLED_RST_Pin);
        break;

    default:
        return 0; //A message was received which is not implemented, return 0 to indicate an error
    }

    return 1; // command processed successfully.
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    u8g2_t u8g2;
    u8 t;
    u8 len;
    u16 times = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //延时初始化
    uart_init(115200);                              //串口初始化波特率为115200
    MX_GPIO_Init();
    LED_Init();

    u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_4wire_sw_spi, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(&u8g2);     // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display

    GPIO_ResetBits(OLED_RST_GPIO_Port, OLED_RST_Pin);
    delay_ms(100);
    GPIO_SetBits(OLED_RST_GPIO_Port, OLED_RST_Pin);
    delay_ms(200);

    delay_ms(1000);

    //u8g2_DrawLine(&u8g2, 50, 50, 100, 100);
    //u8g2_SendBuffer(&u8g2);

    u8g2_FirstPage(&u8g2);
    do
    {
        u8g2_DrawLine(&u8g2, 0, 0, 128, 64);
    } while (u8g2_NextPage(&u8g2));

    while (1)
    {
        if (USART1_RX_STA & 0x8000)
        {
            len = USART1_RX_STA & 0x3fff; //得到此次接收到的数据长度
            printf("\r\nyour message is:\r\n");
            for (t = 0; t < len; t++)
            {
                USART_SendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
                while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
                    ; //等待发送结束
            }
            printf("\r\n\r\n"); //插入换行
            USART1_RX_STA = 0;
        }
        else
        {
            times++;
            if (times % 5000 == 0)
            {
                printf("\r\nALIENTEK_STM32F407_USART\r\n");
                printf("@ALIENTEK\r\n\r\n\r\n");
            }
            if (times % 200 == 0)
                printf("input data\r\n");
            if (times % 30 == 0)
                LED0 = !LED0; //闪烁LED,提示系统正在运行.
            delay_ms(10);
        }
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //使能PD端口时钟

    GPIO_InitStructure.GPIO_Pin = OLED_CS_Pin | OLED_RST_Pin | OLED_DC_Pin | OLED_MOSI_Pin | OLED_SCK_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, OLED_CS_Pin | OLED_RST_Pin | OLED_DC_Pin | OLED_MOSI_Pin | OLED_SCK_Pin);
}
