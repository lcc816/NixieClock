/**
  ******************************************************************************
  * @file    key.h
  * @author  Lichangchun
  * @version 1.0
  * @date    13-Jun-2019
  * @brief   按键驱动头文件
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H
#define __KEY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported define -----------------------------------------------------------*/
/* 按键状态 */
#define KEY0_PRESS  1   // KEY0 按下
#define KEY1_PRESS  2   // KEY1 按下
#define KEY2_PRESS  3   // KEY2 按下

#define N_KEY     // 未按下
#define S_KEY     // 单击
#define L_KEY     // 长按

#define KEY0_PIN                        GPIO_Pin_4
#define KEY0_GPIO_PORT                  GPIOA
#define KEY0_GPIO_CLK                   RCC_APB2Periph_GPIOA

#define KEY1_PIN                        GPIO_Pin_5
#define KEY1_GPIO_PORT                  GPIOA
#define KEY1_GPIO_CLK                   RCC_APB2Periph_GPIOA

#define KEY2_PIN                        GPIO_Pin_6
#define KEY2_GPIO_PORT                  GPIOA
#define KEY2_GPIO_CLK                   RCC_APB2Periph_GPIOA


/* Exported functions ------------------------------------------------------- */
void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);

#endif /* __KEY_H */
