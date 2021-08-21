/*******************************************************************************
 * @file    hc02.h
 * @author  LCC
 * @version V1.0
 * @date    21-Aug-2021
 * @brief
 *******************************************************************************/

#ifndef __HC02_H
#define __HC02_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported define -----------------------------------------------------------*/
#define HC02_KEY_PIN            GPIO_Pin_1
#define HC02_KEY_PORT           GPIOA
#define HC02_KEY_PCC            RCC_APB2Periph_GPIOA

#define HC02_STA_PIN            GPIO_Pin_0
#define HC02_STA_PORT           GPIOA
#define HC02_STA_PCC            RCC_APB2Periph_GPIOA

#define IS_HC02_CONNECTED   (GPIO_ReadInputDataBit(HC02_STA_PORT, HC02_STA_PIN) > 0)

#endif /* __HC02_H */
