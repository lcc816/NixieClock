/*******************************************************************************
* @file     --> buzzer.h
* @author   --> Lichangchun
* @version  --> v1.0
* @date     --> 13-Jun-2019
* @brief    --> PC6 <-> TIM8_CH1
*******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUZZER_H
#define __BUZZER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported functions ------------------------------------------------------- */
void Buzzer_Init(void);
void Buzzer_Sound1(void);
/* Exported functions ------------------------------------------------------- */

#endif /* __BUZZER_H */
