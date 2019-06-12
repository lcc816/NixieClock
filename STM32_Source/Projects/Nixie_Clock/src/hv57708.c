/**
  ******************************************************************************
  * @file    hv57708.c
  * @author  Lichangchun
  * @version 1.0
  * @date    11-Jun-2019
  * @brief   HV57708 驱动
  *           芯片简介: 
  *           32MHz, 64通道推挽输出串入并出芯片, 
  *           内有 4 个 并行的 16 位移位寄存器
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "hv57708.h"
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  HV57708 初始化
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_Init(void)
{
  GPIO_InitTypeDef	GPIO_InitStructure;
	/* CLK, LE, POL */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* 数据引脚 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  
/******************************************************************************/

	HV_POL_H;

	HV_DIN1_L;
	HV_DIN2_L;
	HV_DIN3_L;
	HV_DIN4_L;

	HV_CLK_L;
	HV_LE_L;
}

/*******************************************************************************
  * @brief  向 HV57708 发送 64 位数据
  * @param  datapart1 - 第一部分数据, 32 位
  *         datapart2 - 第二部分数据, 32 位
  * @retval None
*******************************************************************************/
void HV57708_SendData(uint32_t datapart2, uint32_t datapart1)
{
  uint8_t i;
  uint32_t tmp;
  tmp = datapart1;
  for (i = 0; i < 8; i++)
  {
    if (tmp & 0x00000001)
      HV_DIN1_H;
    else
      HV_DIN1_L;
    tmp = tmp >> 1;
    if (tmp & 0x00000001)
      HV_DIN2_H;
    else
      HV_DIN2_L;
    tmp = tmp >> 1;
    if (tmp & 0x00000001)
      HV_DIN3_H;
    else
      HV_DIN3_L;
    tmp = tmp >> 1;
    if (tmp & 0x00000001)
      HV_DIN4_H;
    else
      HV_DIN4_L;
    tmp = tmp >> 1;
    
    HV_CLK_H;
    /* 至少 62 ns */
    __nop(); __nop(); __nop(); __nop();; __nop(); __nop();
    HV_CLK_L;
  }
  tmp = datapart2;
  for (i = 0; i < 8; i++)
  {
    if (tmp & 0x00000001)
      HV_DIN1_H;
    else
      HV_DIN1_L;
    tmp = tmp >> 1;
    if (tmp & 0x00000001)
      HV_DIN2_H;
    else
      HV_DIN2_L;
    tmp = tmp >> 1;
    if (tmp & 0x00000001)
      HV_DIN3_H;
    else
      HV_DIN3_L;
    tmp = tmp >> 1;
    if (tmp & 0x00000001)
      HV_DIN4_H;
    else
      HV_DIN4_L;
    tmp = tmp >> 1;
    
    HV_CLK_H;
    __nop(); __nop(); __nop(); __nop();; __nop(); __nop();
    HV_CLK_L;
  }
}

/*******************************************************************************
  * @brief  将 HV57708 寄存器中的数据发送到引脚, 即锁存使能脉冲
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_OutputData(void)
{
  __nop(); __nop();
  HV_LE_H;
  /* 至少 25ns */
  __nop(); __nop();
  HV_LE_L;
  __nop(); __nop();
}

/*******************************************************************************
  * @brief  将 HV57708 寄存器中的数据发送到引脚, 即锁存使能脉冲
  * @param  data: data0 ~ data5 表示从低位到高位
  * @retval None
*******************************************************************************/
void HV57708_Display(unsigned char data[])
{
  uint32_t part2 = 0xFFFFFFFF, part1 = 0xFFFFFFFF;
  uint32_t temp;
  uint16_t pos[6];
  uint8_t i;
  
  for (i = 0; i < 6; i++)
  {
    if (data[i] == 0)
      pos[i] = 0x01FF;
    else
      pos[i] = 0x03FF ^ (1 << (data[i] - 1));
  }
  
  temp = pos[5];
  temp <<= 18;
  temp |= 0xFFC00FF;
  part2 &= temp;
  
  temp = pos[4];
  temp <<= 8;
  temp |= 0xFFFD00FF;
  part2 &= temp;
  
  temp = pos[3];
  temp = temp << 30;
  temp |= 0x3FFFFFFF;
  part1 &= temp;
  
  temp = pos[2];
  temp = temp << 20;
  temp |= 0xC00FFFFF;
  part1 &= temp;
  
  temp = pos[1];
  temp = temp << 10;
  temp |= 0xFFF003FF;
  part1 &= temp;
  
  temp = pos[0];
  temp |= 0xFFFFFD00;
  part1 &= temp;
  
  HV57708_SendData(part2, part1);
	HV57708_OutputData();
}
