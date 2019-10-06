/*******************************************************************************
* @file     --> display.h
* @author   --> Lichangchun
* @version  --> 1.0
* @date     --> 5-Oct-2019
* @brief    --> 此头文件整理了处理各种显示操作的函数
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H
#define	__DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "ds3231.h"

/* Exported functions ------------------------------------------------------- */

void Display_Init(void);

void Time_Display(Time_TypeDef *time);
void Date_Display(Time_TypeDef *time);
void TempOrHumi_Display(float value);

#endif /* __DISPLAY_H */
