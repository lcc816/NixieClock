/**
  ******************************************************************************
  * @file    led.h
  * @author  Lichangchun
  * @version 
  * @date    6-Sept-2017
  * @brief   
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
#define LED_PIN                         GPIO_Pin_8
#define LED_GPIO_PORT                   GPIOA
#define LED_GPIO_CLK                    RCC_APB2Periph_GPIOA

/* Exported functions ------------------------------------------------------- */
void LED_Init(void);
void LED_Off(void);
void LED_On(void);
void LED_Toggle(void);

#endif /* __LED_H */
