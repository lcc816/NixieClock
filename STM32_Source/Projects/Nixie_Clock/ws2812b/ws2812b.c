/**
  ******************************************************************************
  * @file    ws2812b.c
  * @author  Lichangchun
  * @version 1.0
  * @date    30-Jun-2021
  * @brief   WS2812B 全彩灯珠驱动
  *         硬件 SPI 模拟 WS2812B 时序, SPI 的 16 位数据表示 WS2812B 的 1 位.
  *         STM32 主频 72 MHz, 预分频 4, 则 SPI 一位数据传输时间为 55.6 ns,
  *         SPI 的 5 bits 持续时长约 278 ns, 11 bits 持续时长约 612 ns, 所以将
  *         SPI 的 2 bytes 按 5:11 或 11:5 划分则分别可表示 WS2812B 的 0 码和
  *         1 码, 这样, 一个像素点需要 3 * 8 * 2 = 48 个字节 SPI 数据表示.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ws2812b.h"
#include "delay.h"

/* Private define ------------------------------------------------------------*/
#define OneBitOccupyByte    2 /* 一位占用多少个字节 */
#define OneNodeBuffLength   8 * 3 * OneBitOccupyByte

/* Private variables ---------------------------------------------------------*/
uint8_t pixelBuffer[WS2812B_NUM * OneNodeBuffLength] = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  WS2812B 端口初始化
  * @param  None
  * @retval None
*******************************************************************************/
void WS2812B_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //PORTA时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); //SPI1时钟使能
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

    /* PA7  SPI1_MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PA7复用推挽输出 SPI
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第2个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		//定义波特率预分频的值:4分频<->18MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI1, ENABLE); //使能SPI外设
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

    DMA_DeInit(DMA1_Channel3);   //将DMA的通道1寄存器重设为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(SPI1 -> DR); //cpar;  //DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pixelBuffer; //cmar;  //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
    DMA_InitStructure.DMA_BufferSize = sizeof(pixelBuffer); //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道DMA_Channel13所标识的寄存器

    WS2812B_Clear();  //关闭全部的灯
    delay_ms(100); //关闭全部的灯需要一定的时间
}

/*******************************************************************************
  * @brief  WS2812B 更新显示一遍
  * @param  None
  * @retval None
*******************************************************************************/
void WS2812B_Show(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE); //关闭USART1 TX DMA1 所指示的通道
    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_SetCurrDataCounter(DMA1_Channel3, sizeof(pixelBuffer));//DMA通道的DMA缓存的大小
    DMA_Cmd(DMA1_Channel3, ENABLE);  //使能 DMA1_Channel3 所指示的通道
}

/*******************************************************************************
  * @brief  将三原色(RGB)合并为 24 位颜色数据
  * @param  r, g, b
  * @retval 颜色数据
*******************************************************************************/
uint32_t WS2812B_ConvertColor(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

/*******************************************************************************
  * @brief  设置第 n 个灯珠颜色
  * @param  n 灯珠在灯带中的偏移
  * @param  color 颜色数值, 最高 8 位全 0, 随后每 8 位依次表示 r, g, b
  * @retval None
*******************************************************************************/
void WS2812B_SetPixelColor(uint16_t n, uint32_t color)
{
    uint8_t i = 24;
    uint8_t *buff;

    if (n >= WS2812B_NUM)
    {
        return;
    }

    buff = &pixelBuffer[OneNodeBuffLength * n];
    while (i--)
    {
        //  bit 1: 1111 1111 1110 0000
        if (color & 0x800000)
        {
            *buff++ = 0xff;
            *buff++ = 0xe0;
        }
        //  bit 0: 1111 1000 0000 0000
        else
        {
            *buff++ = 0xf8;
            *buff++ = 0x00;
        }
        //  bit 1: 11bit h, bit 0: 5bit h
        color <<= 1;
    }
}

/*******************************************************************************
  * @brief  通过 RGB 设置第 n 个灯珠颜色
  * @param  n 灯珠位置
  *         r 红色数值
  *         g 绿色数值
  *         b 蓝色数值
  * @retval None
*******************************************************************************/
void WS2812B_SetPixelRGB(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t color;

    if (n >= WS2812B_NUM)
    {
        return;
    }

    color = WS2812B_ConvertColor(r, g, b);
    WS2812B_SetPixelColor(n, color);
}

/*******************************************************************************
  * @brief  将所有灯珠设置为同一颜色
  * @param  r 红色数值
  *         g 绿色数值
  *         b 蓝色数值
  * @retval None
*******************************************************************************/
void WS2812B_SetPixelRGBAll(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t i;
    for (i = 0; i < WS2812B_NUM; i++)
    {
        WS2812B_SetPixelRGB(i, r, g, b);
    }
}

/*******************************************************************************
  * @brief  WS2812B 关闭所有灯珠
  * @param  None
  * @retval None
*******************************************************************************/
void WS2812B_Clear(void)
{
    WS2812B_SetPixelRGBAll(0, 0, 0);
    WS2812B_Show();
}

/**************** 移植 Adafruit_NeoPixel 的库函数部分 *************************/

/*******************************************************************************
  * @brief  获取按 r - g - b - r 轮转的颜色数值
  * @param  wheelPos 0~255 的数值用以获取颜色数值
  * @retval 颜色数值
*******************************************************************************/
uint32_t WS2812B_Wheel(uint8_t wheelPos)
{
    wheelPos = 255 - wheelPos;
    if(wheelPos < 85)
    {
        return WS2812B_ConvertColor(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if(wheelPos < 170)
    {
        wheelPos -= 85;
        return WS2812B_ConvertColor(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return WS2812B_ConvertColor(wheelPos * 3, 255 - wheelPos * 3, 0);
}

/*******************************************************************************
  * @brief  用颜色依次填充每个点
  * @param  c 要填充的颜色
  *         wait 间隔时间, ms
  * @retval None
*******************************************************************************/
void WS2812B_ColorWipe(uint32_t c, uint8_t wait)
{
    for(uint16_t i=0; i < WS2812B_NUM; i++)
    {
        WS2812B_SetPixelColor(i, c);
        WS2812B_Show();
        delay_ms(wait);
    }
}

/*******************************************************************************
  * @brief  显示彩虹效果
  * @param  wait 间隔时间, ms
  * @retval None
*******************************************************************************/
void WS2812B_Rainbow(uint8_t wait)
{
    uint16_t i, j;

    for(j = 0; j < 256; j++)
    {
        for(i = 0; i < WS2812B_NUM; i++)
        {
            WS2812B_SetPixelColor(i, WS2812B_Wheel((i+j) & 255));
        }
        WS2812B_Show();
        delay_ms(wait);
    }
}

/*******************************************************************************
  * @brief  均匀分布的彩虹效果
  * @param  wait 间隔时间, ms
  * @retval None
*******************************************************************************/
void WS2812B_RainbowCycle(uint8_t wait)
{
    uint16_t i, j;

    for(j = 0; j < 256 * 5; j++)
    {   // 5 cycles of all colors on wheel
        for(i = 0; i < WS2812B_NUM; i++)
        {
            WS2812B_SetPixelColor(i, WS2812B_Wheel(((i * 256 / WS2812B_NUM) + j) & 255));
        }
        WS2812B_Show();
        delay_ms(wait);
    }
}

/*******************************************************************************
  * @brief  影院风格
  * @param  c 要填充的颜色
  *         wait 间隔时间, ms
  * @retval None
*******************************************************************************/
void WS2812B_TheaterChase(uint32_t c, uint8_t wait)
{
    for (int j = 0; j < 10; j++)
    {   //do 10 cycles of chasing
        for (int q = 0; q < 3; q++)
        {
            for (uint16_t i = 0; i < WS2812B_NUM; i=i+3)
            {
                WS2812B_SetPixelColor(i + q, c);//turn every third pixel on
            }
            WS2812B_Show();

            delay_ms(wait);

            for (uint16_t i=0; i < WS2812B_NUM; i=i+3)
            {
                WS2812B_SetPixelColor(i+q, 0);//turn every third pixel off
            }
        }
    }
}

/*******************************************************************************
  * @brief  带彩虹效果的影院爬行灯
  * @param  wait 间隔时间, ms
  * @retval None
*******************************************************************************/
void WS2812B_TheaterChaseRainbow(uint8_t wait) {
    for (int j = 0; j < 256; j++)
    {   // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++)
        {
            for (uint16_t i = 0; i < WS2812B_NUM; i = i + 3)
            {
                WS2812B_SetPixelColor(i + q, WS2812B_Wheel( (i + j) % 255));    //turn every third pixel on
            }
            WS2812B_Show();

            delay_ms(wait);

            for (uint16_t i = 0; i < WS2812B_NUM; i = i + 3)
            {
                WS2812B_SetPixelColor(i + q, 0); //turn every third pixel off
            }
        }
    }
}
