/**
  ******************************************************************************
  * @file    hv57708.h
  * @author  Lichangchun
  * @version 1.0
  * @date    11-Jun-2019
  * @brief   
  *     HV57708_CLK - CLK
  *     HV57708_LE  - LE
  *     HV57708_POL - POL
  *     HV57708_DI1 - Din1/Dout4(A)
  *     HV57708_DI2 - Din2/Dout3(A)
  *     HV57708_DI3 - Din3/Dout2(A)
  *     HV67708_DI4 - Din4/Dout1(A)
  ******************************************************************************
  */
  
/*
第一部分
   |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 1 | 1-1 | 1-5 | 1-9 | 2-3 | 2-7 | 3-1 | 3-5 | 3-9 |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 2 | 1-2 | 1-6 | 1-0 | 2-4 | 2-8 | 3-2 | 3-6 | 3-0 |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 3 | 1-3 | 1-7 | 2-1 | 2-5 | 2-9 | 3-3 | 3-7 | 4-1 |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 4 | 1-4 | 1-8 | 2-2 | 2-6 | 2-0 | 3-4 | 3-8 | 4-2 |
 
 第二部分
   |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 1 | 4-3 | 4-7 | 5-1 | 5-5 | 5-9 | 6-3 | 6-7 |     |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 2 | 4-4 | 4-8 | 5-2 | 5-6 | 5-0 | 6-4 | 6-8 |     |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 3 | 4-5 | 4-9 | 5-3 | 5-7 | 6-1 | 6-5 | 6-9 |     |
---+-----+-----+-----+-----+-----+-----+-----+-----+
 4 | 4-6 | 4-0 | 5-4 | 5-8 | 6-2 | 6-6 | 6-0 |     |
 
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HV57708_H
#define __HV57708_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define HV57708_CLK_H      GPIO_SetBits(GPIOC, GPIO_Pin_11)
#define HV57708_LE_H       GPIO_SetBits(GPIOC, GPIO_Pin_12)
#define HV57708_POL_H      GPIO_SetBits(GPIOC, GPIO_Pin_10)
#define HV57708_DIN1_H     GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define HV57708_DIN2_H     GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define HV57708_DIN3_H     GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define HV57708_DIN4_H     GPIO_SetBits(GPIOB, GPIO_Pin_12)

#define HV57708_CLK_L      GPIO_ResetBits(GPIOC, GPIO_Pin_11)
#define HV57708_LE_L       GPIO_ResetBits(GPIOC, GPIO_Pin_12)
#define HV57708_POL_L      GPIO_ResetBits(GPIOC, GPIO_Pin_10)
#define HV57708_DIN1_L     GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define HV57708_DIN2_L     GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define HV57708_DIN3_L     GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define HV57708_DIN4_L     GPIO_ResetBits(GPIOB, GPIO_Pin_12)

/* Exported functions ------------------------------------------------------- */
void HV57708_Init(void);
void HV57708_TubePower(FunctionalState NewState);
void HV57708_SendData(uint32_t datapart2, uint32_t datapart1);
void HV57708_OutputData(void);
void HV57708_Display(unsigned char data[]);
void HV57708_Protection(void);
/*测试用*/
void HV57708_Scan(void);
void HV57708_SetPin(uint8_t pin);

#endif /* __HV57708_H */
