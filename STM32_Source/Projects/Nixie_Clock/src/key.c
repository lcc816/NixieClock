/**
  ******************************************************************************
  * @file     key.c
  * @author   Lichangchun
  * @version  1.0
  * @date     12-Jun-2019
  * @brief    按键驱动
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "delay.h"

/* Private define ------------------------------------------------------------*/
#define KEY0 GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_PIN) // 读取按键 0
#define KEY1 GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_PIN) // 读取按键 1
#define KEY2 GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_PIN) // 读取按键 2

/*******************************************************************************
  * @brief  按键 GPIO 配置
  * @param  None
  * @retval None
*******************************************************************************/
void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* 使能端口时钟 */
  RCC_APB2PeriphClockCmd(KEY0_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(KEY0_GPIO_CLK, ENABLE); 
  RCC_APB2PeriphClockCmd(KEY0_GPIO_CLK, ENABLE); 
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 设置成上拉输入
  
  GPIO_InitStructure.GPIO_Pin = KEY0_PIN;
  GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure); // KEY0
  
  GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
  GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure); // KEY1
  
  GPIO_InitStructure.GPIO_Pin = KEY2_PIN;
  GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure); // KEY2
}

/*******************************************************************************
  * @brief  按键扫描函数
  * @param  mode: 按键模式, 0-不支持连续按; 1-支持连续按
  * @retval 按键值
  *           KEY0_PRESS: KEY0 被按下
  *           KEY1_PRESS: KEY1 被按下
  *           KEY2_PRESS: KEY2 被按下
  *           0: 无按键按下
*******************************************************************************/
uint8_t KEY_Scan(uint8_t mode)
{
  static uint8_t key_up = 1; // 按键松开标志
  if (mode) key_up = 1;
  
  if (key_up && (KEY0==0 | KEY1==0 | KEY2==0))
  {
    delay_ms(10);   // 去抖动
    key_up = 0;
    if (KEY0 == 0)
      return KEY0_PRESS;
    else if (KEY1 == 0)
      return KEY1_PRESS;
    else if (KEY2 == 0)
      return KEY2_PRESS;
  }
  else if (KEY0==1 && KEY1==1 && KEY2==1)
    key_up = 1;
  
  return 0;
}
