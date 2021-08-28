/**
 ******************************************************************************
 * @file    usart.c
 * @author  Lichangchun
 * @version
 * @date    13-April-2017
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/*******************************************************************************
 * @brief  串口初始化
 * @param  bound: 波特率
 * @retval None
 *******************************************************************************/
void USART1_Configuration(uint32_t bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA.9

    //USART1_RX  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA.10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;	//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启串口接收中断
    USART_Cmd(USART1, ENABLE);
}

#ifdef __GNUC__
int _write(int fd, char *str, int len)
{
    /* implement your write code here */
    char *p = str;
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
        /* 此句保证串口打印的第一个字符不会丢失 */
        USART_ClearFlag(USART1, USART_FLAG_TC);
        USART_SendData(USART1, *p);
        /* 等待发送完毕 */
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        p++;
    }
    return len;
}
#else /* __GNUC__ */
/*******************************************************************************
 * @brief  重定向c库函数到USART1
 *******************************************************************************/
int fputc(int ch, FILE *f)
{
    /* 清 SR 寄存器中的 TC 标志 */
    /* 此句保证串口打印的第一个字符不会丢失 */
    USART_ClearFlag(USART1, USART_FLAG_TC);

    /* 发送一个字节数据到 USART1 */
    USART_SendData(USART1, (uint8_t)ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;

    return (ch);
}

/*******************************************************************************
 * @brief  重定向c库函数 scanf 到 USART1
 *******************************************************************************/
int fgetc(FILE *f)
{
    // 等待串口1输入数据
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        ;

    return (int)USART_ReceiveData(USART1);
}
#endif /* __GNUC__ */

/*******************************************************************************
 * @brief  USART1发送一个字节
 * @param  Data 要发送的字节数据
 * @retval None
 *******************************************************************************/
void USART1_SendData8(uint8_t Data)
{
    USART_SendData(USART1, Data);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
}

/*******************************************************************************
 * @brief  USART1收中断处理函数
 * @param  None
 * @retval None
 *******************************************************************************/
void USART1_IRQHandler(void)
{
    uint8_t val;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* 读取接收数据寄存器 USART1->DR */
        val = USART_ReceiveData(USART1);
        /* do something ... */
        if (val)
        {
        }
    }
}
