//https://blog.csdn.net/qq_35281599/article/details/80299770

#include "sys.h"
#include "usart2.h"
//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h" //ucos 使用
#endif

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0)
        ; //循环发送,直到发送完毕
    USART1->DR = (u8)ch;
    return ch;
}
#endif

#if EN_USART1_RX //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0; //接收状态标记
u8 Uart2_Buffer[Max_BUFF_Len] = {0};
u16 Uart2_Rx = 0;
//初始化IO 串口1
//bound:波特率
void uart_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //使能USART1时钟

    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10复用为USART1

    //USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化PA9，PA10

    //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;                                     //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(USART1, &USART_InitStructure);                                       //初始化串口1

    USART_Cmd(USART1, ENABLE); //使能串口1

    //USART_ClearFlag(USART1, USART_FLAG_TC);

#if EN_USART1_RX
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启相关中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;         //串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器、

#endif
}

void USART1_IRQHandler(void) //串口1中断服务程序
{
    u8 Res;
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntEnter();
#endif
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART1); //(USART1->DR);	//读取接收到的数据

        if ((USART_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (USART_RX_STA & 0x4000) //接收到了0x0d
            {
                if (Res != 0x0a)
                    USART_RX_STA = 0; //接收错误,重新开始
                else
                    USART_RX_STA |= 0x8000; //接收完成了
            }
            else //还没收到0X0D
            {
                if (Res == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }
    }
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntExit();
#endif
}
#endif

void USART2_IRQHandler()
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //中断产生
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); //清除中断标志

        Uart2_Buffer[Uart2_Rx] = USART_ReceiveData(USART2); //接收串口1数据到buff缓冲区
        Uart2_Rx++;

        if (Uart2_Buffer[Uart2_Rx - 1] == 0x0a || Uart2_Rx == Max_BUFF_Len) //如果接收到尾标识是换行符（或者等于最大接受数就清空重新接收）
        {
            if (Uart2_Buffer[0] == '+') //检测到头标识是我们需要的
            {
                printf("%s\r\n", Uart2_Buffer); //这里我做打印数据处理
                Uart2_Rx = 0;
            }
            else
            {
                Uart2_Rx = 0; //不是我们需要的数据或者达到最大接收数则开始重新接收
            }
        }
    }
}

/*数据的头标识为“\n”既换行符，尾标识为“+”。该函数将串口接收的数据存放在USART_Buffer数组中，然后先判断当前字符是不是尾标识，如果是说明接收完毕，然后再来判断头标识是不是“+”号，如果还是那么就是我们想要的数据，接下来就可以进行相应数据的处理了。但如果不是那么就让Usart2_Rx=0重新接收数据。这样做的有以下好处：

        1.可以接受不定长度的数据，最大接收长度可以通过Max_BUFF_Len来更改

        2.可以接受指定的数据

        3.防止接收的数据使数组越界
        这里我的把接受正确数据直接打印出来，也可以通过设置标识位，然后在主函数里面轮询再操作。

*/