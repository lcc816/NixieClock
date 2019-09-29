/*******************************************************************************
* @file     --> neon.c
* @author   --> Lichangchun
* @version  --> v1.0
* @date     --> 29-Sept-2019
* @brief    --> 氖泡驱动
*******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "neon.h"

/* Private variables ---------------------------------------------------------*/
FlagStatus Neon_Status = RESET;     // 记录氖泡亮灭状态

/*******************************************************************************
* @brief    --> 氖泡初始化函数
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Neon_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  Neon_Status = RESET; // 熄灭所有氖泡
  GPIO_ResetBits(GPIOC, GPIO_Pin_0); // DOT1
  GPIO_ResetBits(GPIOC, GPIO_Pin_1); // DOT2
  GPIO_ResetBits(GPIOC, GPIO_Pin_2); // DOT3
  GPIO_ResetBits(GPIOC, GPIO_Pin_3); // DOT4
}

/*******************************************************************************
* @brief    --> 点亮指定氖泡
* @param    --> dot: 取值 DOT1_BIT, DOT2_BIT, DOT3_BIT, DOT4_BIT
                      可进行或运算
* @retval   --> None
*******************************************************************************/
void Neon_On(uint8_t dot)
{
  if (dot & DOT1_BIT)
    GPIO_SetBits(GPIOC, GPIO_Pin_0);
  
  if (dot & DOT2_BIT)
    GPIO_SetBits(GPIOC, GPIO_Pin_1);
  
  if (dot & DOT3_BIT)
    GPIO_SetBits(GPIOC, GPIO_Pin_2);
  
  if (dot & DOT4_BIT)
    GPIO_SetBits(GPIOC, GPIO_Pin_3);
}

/*******************************************************************************
* @brief    --> 熄灭指定氖泡
* @param    --> dot: 取值 DOT1_BIT, DOT2_BIT, DOT3_BIT, DOT4_BIT
                      可进行或运算
* @retval   --> None
*******************************************************************************/
void Neon_Off(uint8_t dot)
{
  if (dot & DOT1_BIT)
    GPIO_ResetBits(GPIOC, GPIO_Pin_0);
  
  if (dot & DOT2_BIT)
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);
  
  if (dot & DOT3_BIT)
    GPIO_ResetBits(GPIOC, GPIO_Pin_2);
  
  if (dot & DOT4_BIT)
    GPIO_ResetBits(GPIOC, GPIO_Pin_3);
}

/*******************************************************************************
* @brief    --> 翻转指定氖泡
* @param    --> dot: DOT1_BIT, DOT2_BIT, DOT3_BIT, DOT4_BIT
* @retval   --> None
*******************************************************************************/
void Neon_Flip(uint8_t dot)
{
  if (dot & DOT1_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_0))
      GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    else 
      GPIO_SetBits(GPIOC, GPIO_Pin_0);
  }
  if (dot & DOT2_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1))
      GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    else 
      GPIO_SetBits(GPIOC, GPIO_Pin_1);
  }
  if (dot & DOT3_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_2))
      GPIO_ResetBits(GPIOC, GPIO_Pin_2);
    else 
      GPIO_SetBits(GPIOC, GPIO_Pin_2);
  }
  if (dot & DOT4_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_3))
      GPIO_ResetBits(GPIOC, GPIO_Pin_3);
    else 
      GPIO_SetBits(GPIOC, GPIO_Pin_3);
  }
}

/*******************************************************************************
* @brief    --> 点亮所有氖泡
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Neon_AllOn(void)
{
  GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
  Neon_Status = SET;
}

/*******************************************************************************
* @brief    --> 熄灭所有氖泡
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Neon_AllOff(void)
{
  GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
  Neon_Status = RESET;
}

/*******************************************************************************
* @brief    --> 统一翻转所有氖泡
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Neon_FlipAll(void)
{
  if (Neon_Status)
    Neon_AllOff();
  else
    Neon_AllOn();
}
