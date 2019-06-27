/*******************************************************************************
* @file     --> key.h
* @author   --> Lichangchun
* @version  --> v1.1
* @date     --> 22-Jun-2019
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

#define KEY0 GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_PIN) // 读取按键 0
#define KEY1 GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_PIN) // 读取按键 1
#define KEY2 GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_PIN) // 读取按键 2

/* 按键引脚定义, 根据实际电路更改 */
#define KEY0_PIN                GPIO_Pin_4
#define KEY0_GPIO_PORT          GPIOA
#define KEY0_GPIO_CLK           RCC_APB2Periph_GPIOA
#define KEY0_PRESS_LEVEL        0                 // 表示按下的电平
#define KEY0_EXTI_Line          EXTI_Line4        // 外部中断线
#define KEY0_EXIT_IRQn          EXTI4_IRQn 
#define KEY0_GPIO_PortSource    GPIO_PortSourceGPIOA
#define KEY0_GPIO_PinSource     GPIO_PinSource4

#define KEY1_PIN                GPIO_Pin_5
#define KEY1_GPIO_PORT          GPIOA
#define KEY1_GPIO_CLK           RCC_APB2Periph_GPIOA
#define KEY1_PRESS_LEVEL        0
#define KEY1_EXTI_Line          EXTI_Line5
#define KEY1_EXIT_IRQn          EXTI9_5_IRQn // 外部中断通道, 注意 5-9 和 10-15 分别共用一个通道
#define KEY1_GPIO_PortSource    GPIO_PortSourceGPIOA
#define KEY1_GPIO_PinSource     GPIO_PinSource5

#define KEY2_PIN                GPIO_Pin_6
#define KEY2_GPIO_PORT          GPIOA
#define KEY2_GPIO_CLK           RCC_APB2Periph_GPIOA
#define KEY2_PRESS_LEVEL        0
#define KEY2_EXTI_Line          EXTI_Line6
#define KEY2_EXIT_IRQn          EXTI9_5_IRQn
#define KEY2_GPIO_PortSource    GPIO_PortSourceGPIOA
#define KEY2_GPIO_PinSource     GPIO_PinSource6

// !!! 注意根据引脚所在的中断线修改 key.c 中的外部中断服务函数名 !!!

/* Exported variables --------------------------------------------------------*/
extern volatile uint8_t key0_short_flag;
extern volatile uint8_t key0_long_flag;
extern volatile uint8_t key1_short_flag;
extern volatile uint8_t key1_long_flag;
extern volatile uint8_t key2_short_flag;
extern volatile uint8_t key2_long_flag;

/* Exported functions ------------------------------------------------------- */
void KEY_Init(void);
void KEY_EXTI_ITConfig(uint32_t EXTI_Line, FunctionalState NewState);
uint8_t KEY_Scan(void);

#endif /* __KEY_H */
