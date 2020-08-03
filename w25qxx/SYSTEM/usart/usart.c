#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "w25qxx.h"
//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h" //ucos 使用
#endif
//////////////////////////////////////////////////////////////////////////////////

//串口1初始化
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com
//********************************************************************************
//V1.3修改说明
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
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
_sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while ((USART6->SR & 0X40) == 0)
        ; //循环发送,直到发送完毕
    USART6->DR = (u8)ch;
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

//usart6
u32 GBK_OVER_Flag = 0;
u8 GBK_BUF_Flag = 2;
u8 Usart6_Rece_Buf0[Usart6_DMA_Len + 1];
u8 Usart6_Rece_Buf1[Usart6_DMA_Len + 1];
u16 Usart6_Rec_Len = 0;

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

    USART_ClearFlag(USART1, USART_FLAG_TC);

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

// void USART1_IRQHandler(void) //串口1中断服务程序
// {
//     u8 Res;
// #ifdef OS_TICKS_PER_SEC //如果时钟节拍数定义了,说明要使用ucosII了.
//     OSIntEnter();
// #endif
//     if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//     {
//         Res = USART_ReceiveData(USART1); //(USART1->DR);	//读取接收到的数据

//         if ((USART_RX_STA & 0x8000) == 0) //接收未完成
//         {
//             if (USART_RX_STA & 0x4000) //接收到了0x0d
//             {
//                 if (Res != 0x0a)
//                     USART_RX_STA = 0; //接收错误,重新开始
//                 else
//                     USART_RX_STA |= 0x8000; //接收完成了
//             }
//             else //还没收到0X0D
//             {
//                 if (Res == 0x0d)
//                     USART_RX_STA |= 0x4000;
//                 else
//                 {
//                     USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
//                     USART_RX_STA++;
//                     if (USART_RX_STA > (USART_REC_LEN - 1))
//                     {
//                         LED0 = 0;
//                         delay_ms(1000);
//                         USART_RX_STA = 0; //接收数据错误,重新开始接收
//                     }
//                 }
//             }
//         }
//     }
// #ifdef OS_TICKS_PER_SEC //如果时钟节拍数定义了,说明要使用ucosII了.
//     OSIntExit();
// #endif
// }
uint32_t WriteAddress = 0;
uint8_t i = 0;
void USART1_IRQHandler(void)
{
    // uint8_t mdata[4096] = 0;
    uint8_t mdata = 0;
    // const char *hdata = 0;
    // uint16_t i = 0;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // mdata[i++] = USART_ReceiveData(USART1);
        mdata = USART_ReceiveData(USART1);
        // USART_RX_STA = 1;
        // SPI_FLASH_BufferWrite(&mdata, WriteAddress++, 1);
        // W25QXX_Write(&mdata, WriteAddress++, 1);
        // W25X_WriteBytes(WriteAddress++, &mdata, 1);
        W25QXX_Write_NoCheck(&mdata, WriteAddress++, 1);
    }
}

//初始化IO 串口6
//bound:波特率
void uart6_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  //使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); //使能USART6时钟

    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //GPIOC6复用为USART6
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //GPIOC7复用为USART6

    //USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA6与GPIOA7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);                 //初始化PC6，PC7

    //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;                                     //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(USART6, &USART_InitStructure);                                       //初始化串口1

    USART_Cmd(USART6, ENABLE); //使能串口1
    USART_ClearFlag(USART6, USART_FLAG_TC);
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); //开启相关中断
    USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;         //串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器、

    //DMA配置
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); //DMA2 时钟使能
    DMA_DeInit(DMA2_Stream1);                            //恢复默认值 串口1接收是DMA2数据流2通道4
    while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE)
    {
    } //等待 DMA 可配置
    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel = DMA_Channel_5;                                      //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART6->DR;                        //DMA 外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)Usart6_Rece_Buf0;                      //DMA 存储器 0 地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                             //外设到存储器模式
    DMA_InitStructure.DMA_BufferSize = Usart6_DMA_Len;                                  //数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                    //外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                             //存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;             //外设数据长度:8 位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                     //存储器数据长度:8 位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                     //注意：这里设置为循环模式，不然不能启动第二次传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                               //中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                              //FIFO 模式禁止
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;                       //FIFO 阈值
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                         //存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;                 //外设突发单次传输
    DMA_DoubleBufferModeConfig(DMA2_Stream1, (uint32_t)Usart6_Rece_Buf1, DMA_Memory_0); //Usart6_Rece_Buf0 先缓冲
    DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);

    DMA_Init(DMA2_Stream1, &DMA_InitStructure); //初始化 DMA Stream
    DMA_Cmd(DMA2_Stream1, ENABLE);              //开启 DMA 传输

    DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE); //使能DMA传输完成中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;   //DMA2_Stream1_IRQn中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器、
}

//开启一次 DMA 传输
//DMA_Streamx:DMA 数据流,DMA1_Stream0~7/DMA2_Stream0~7
//ndtr:数据传输量
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr)
{
    DMA_Cmd(DMA_Streamx, DISABLE); //关闭 DMA 传输
    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
    {
    }                                          //确保 DMA 可以被设置
    DMA_SetCurrDataCounter(DMA_Streamx, ndtr); //数据传输量
    DMA_Cmd(DMA_Streamx, ENABLE);              //开启 DMA 传输
}

void USART6_IRQHandler(void) //串口6中断服务程序
{

    if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        USART_ReceiveData(USART6); //(USART6->DR);	//读取接收到的数据
    }
}

void DMA2_Stream1_IRQHandler(void) //串口1中断服务程序
{
    if (DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TCIF1) == SET)
    {
        DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
        //**********************数据帧处理******************//
        if (1 == DMA_GetCurrentMemoryTarget(DMA2_Stream1))
            GBK_BUF_Flag = 0;
        else
            GBK_BUF_Flag = 1;
        //**************************************************//
    }
}

void send(u8 ch)
{
    while ((USART6->SR & 0X40) == 0)
        ; //循环发送,直到发送完毕
    USART6->DR = (u8)ch;
}

#endif
