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
#include "delay.h"

/*******************************************************************************
  * @brief  HV57708 初始化
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_Init(void)
{
  /* 需要注意的是, HV57708 引脚的驱动电压为 5V, 而 STM32 推挽输出高电平
  仅为 3.3V, 为了匹配电平, STM32 引脚应使用开漏输出并上拉到 5V */
  GPIO_InitTypeDef	GPIO_InitStructure;
	/* CLK, LE, POL */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* 数据引脚 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHZ
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* 辉光管电源开关 */
	/* PB3 复位后功能为 JTDO, 需重映射为普通 IO */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // JTAG-DP 关闭, SW-DP 使能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHZ
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  
/******************************************************************************/

  HV57708_CLK_L;
	HV57708_LE_H;
  HV57708_POL_L; // 反相输出
	HV57708_DIN1_L;
	HV57708_DIN2_L;
	HV57708_DIN3_L;
	HV57708_DIN4_L;
  
  HV57708_SendData(0, 0); // 所有引脚输出低电平
}

/*******************************************************************************
  * @brief  辉光管电源开关
  * @param  NewState - 电源使能(ENABLE)或关闭(DISABLE)
  * @retval None
*******************************************************************************/
void HV57708_TubePower(FunctionalState NewState)
{
  if (NewState != DISABLE)
    GPIO_SetBits(GPIOB, GPIO_Pin_3); /* 打开 */
  else
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
}

/*******************************************************************************
  * @brief  获取辉光管电源开关状态
  * @param  None
  * @retval SET   = 辉光管打开
  *         RESET = 辉光管关闭
*******************************************************************************/
FlagStatus HV57708_TubePowerStatus(void)
{
  return (FlagStatus)GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
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
  tmp = datapart2; // 高位在前
  for (i = 0; i < 8; i++)
  {
    HV57708_CLK_L;
    
    if (tmp & 0x80000000)
      HV57708_DIN4_H;
    else
      HV57708_DIN4_L;
    tmp <<= 1;
    if (tmp & 0x80000000)
      HV57708_DIN3_H;
    else
      HV57708_DIN3_L;
    tmp <<= 1;
    if (tmp & 0x80000000)
      HV57708_DIN2_H;
    else
      HV57708_DIN2_L;
    tmp <<= 1;
    if (tmp & 0x80000000)
      HV57708_DIN1_H;
    else
      HV57708_DIN1_L;
    tmp <<= 1;
    
    HV57708_CLK_H;
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); /* 至少 62 ns */
    HV57708_CLK_L;
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop();
  }
  tmp = datapart1;
  for (i = 0; i < 8; i++)
  {
    HV57708_CLK_L;
    
    if (tmp & 0x80000000)
      HV57708_DIN4_H;
    else
      HV57708_DIN4_L;
    tmp <<= 1;
    if (tmp & 0x80000000)
      HV57708_DIN3_H;
    else
      HV57708_DIN3_L;
    tmp <<= 1;
    if (tmp & 0x80000000)
      HV57708_DIN2_H;
    else
      HV57708_DIN2_L;
    tmp <<= 1;
    if (tmp & 0x80000000)
      HV57708_DIN1_H;
    else
      HV57708_DIN1_L;
    tmp <<= 1;
    
    HV57708_CLK_H;
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); /* 至少 62 ns */
    HV57708_CLK_L;
    __nop(); __nop(); __nop(); __nop(); __nop(); __nop();
  }
}

/*******************************************************************************
  * @brief  将 HV57708 寄存器中的数据发送到引脚, 即锁存使能脉冲
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_OutputData(void)
{
  HV57708_LE_L;
  __nop(); __nop(); __nop();
  HV57708_LE_H;
  /* 至少 25ns */
  __nop(); __nop(); __nop();
  HV57708_LE_L;
}

/*******************************************************************************
  * @brief  将 HV57708 寄存器中的数据发送到引脚, 即锁存使能脉冲
  * @param  data: data0 ~ data5 表示辉光管从左到右
  * @retval None
*******************************************************************************/
void HV57708_Display(uint8_t data[])
{
  assert_param(data != NULL);
  
  if (HV57708_TubePowerStatus() == RESET)
    return;
  
  uint32_t part2 = 0, part1 = 0;
  uint32_t pos[6];
  uint8_t i;
  
  for (i = 0; i < 6; i++)
  {
    if (data[i] == 0)
      pos[i] = 0x0200;
    else
      pos[i] = 0x03FF & (1 << (data[i] - 1));
  }
  
  part1 = pos[0] | pos[1]<<10 | pos[2]<<20 | pos[3]<<30;
  part2 = pos[3]>>2 | pos[4]<<8 | pos[5]<<18;
  
  HV57708_SendData(part2, part1);
	HV57708_OutputData();
}

/*******************************************************************************
  * @brief  阴极保护, 顺序输出一遍所有数字
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_Protection(void)
{
  if (HV57708_TubePowerStatus() == RESET)
    return;
  
  for (uint8_t i = 0; i < 10; i++)
  {
    uint8_t data[6] = {i, i, i, i, i, i};
    HV57708_Display(data);
    delay_ms(75);
  }
  HV57708_SendData(0, 0);
  HV57708_OutputData();
}

/*******************************************************************************
  * @brief  依次设置 1 ~ 64 引脚输出高电平
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_Scan(void)
{
  uint32_t part1=0x00000000, part2=0x00000000;
  for (int i = 0; i < 32; i++)
  {
    part1=0x00000000; part2=0x00000000;
    part1 |= (1 << i);
    HV57708_SendData(part2, part1);
    HV57708_OutputData();
  }
  for (int i = 0; i < 32; i++)
  {
    part1=0x00000000; part2=0x00000000;
    part1 |= (1 << i);
    HV57708_SendData(part2, part1);
    HV57708_OutputData();
    delay_ms(500);
  }
  for (int i = 0; i < 32; i++)
  {
    part1=0x00000000; part2=0x00000000;
    part2 |= (1 << i);
    HV57708_SendData(part2, part1);
    HV57708_OutputData();
    delay_ms(500);
  }
}

/*******************************************************************************
  * @brief  设置某个引脚输出高电平
  * @param  pin - 要输出高电平的引脚(1 ~ 64)
  * @retval None
*******************************************************************************/
void HV57708_SetPin(uint8_t pin)
{
  uint32_t temp = 0x00000000;
  if (pin > 64)
    return;
  if (pin <= 32)
  {
    temp |= (1 << (pin-1));
    HV57708_SendData(0, temp);
    HV57708_OutputData();
  }
  else
  {
    temp |= (1 << (pin-33));
    HV57708_SendData(temp, 0);
    HV57708_OutputData();
  }
}
