/*******************************************************************************
* @file     --> buzzer.h
* @author   --> Lichangchun
* @version  --> v1.0
* @date     --> 27-Jun-2019
* @brief    --> PC8 <-> TIM3_CH3
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUZZER_H
#define __BUZZER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported functions ------------------------------------------------------- */
void Buzzer_Init(void);
void Buzzer_Sound1(void);
void Buzzer_Sound2(void);
/* Exported functions ------------------------------------------------------- */

#endif /* __BUZZER_H */
