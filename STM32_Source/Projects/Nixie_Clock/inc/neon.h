/*******************************************************************************
* @file     --> neon.h
* @author   --> Lichangchun
* @version  --> v1.0
* @date     --> 29-Sept-2019
* @brief    --> 氖泡驱动头文件
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NEON_H
#define __NEON_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported define -----------------------------------------------------------*/
#define DOT1_BIT    0x01 // 0001
#define DOT2_BIT    0x02 // 0010
#define DOT3_BIT    0x04 // 0100
#define DOT4_BIT    0x08 // 1000

/* Exported functions ------------------------------------------------------- */
void Neon_Init(void);
void Neon_On(uint8_t dot);
void Neon_Off(uint8_t dot);
void Neon_Flip(uint8_t dot);
void Neon_AllOff(void);
void Neon_AllOn(void);
void Neon_FlipAll(void);

#endif
