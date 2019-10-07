/*******************************************************************************
* @file     --> key.h
* @author   --> Lichangchun
* @version  --> v1.1
* @date     --> 6-Oct-2019
* @brief    --> 
*******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H
#define __KEY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported define -----------------------------------------------------------*/
/* 按键扫描的返回值 */
#define KEY0_PRESS  1   // KEY0 按下
#define KEY1_PRESS  2   // KEY1 按下
#define KEY2_PRESS  3   // KEY2 按下

/* 按键引脚定义, 根据实际电路更改 */
#define KEY0 GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_PIN) // 读取按键 0
#define KEY1 GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_PIN) // 读取按键 1
#define KEY2 GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_PIN) // 读取按键 2

#define KEY0_PIN                GPIO_Pin_4
#define KEY0_GPIO_PORT          GPIOA
#define KEY0_GPIO_CLK           RCC_APB2Periph_GPIOA
#define KEY0_PRESS_LEVEL        0                 // 表示按下的电平

#define KEY1_PIN                GPIO_Pin_5
#define KEY1_GPIO_PORT          GPIOA
#define KEY1_GPIO_CLK           RCC_APB2Periph_GPIOA
#define KEY1_PRESS_LEVEL        0

#define KEY2_PIN                GPIO_Pin_6
#define KEY2_GPIO_PORT          GPIOA
#define KEY2_GPIO_CLK           RCC_APB2Periph_GPIOA
#define KEY2_PRESS_LEVEL        0

/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void Keys_GPIO_Init(void);
uint8_t Keys_Scan(void);

#endif /* __KEY_H */
