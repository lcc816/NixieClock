/*******************************************************************************
* @file     --> timer.h
* @author   --> Lichangchun
* @version  --> v1.0
* @date     --> 16-Jun-2019
* @brief    --> 
*******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported functions ------------------------------------------------------- */
void TIM3_Int_Init(uint16_t arr, uint16_t psc);
void TIM3_PWM_Init(uint16_t arr, uint16_t psc);

#endif /* __TIMER_H */
