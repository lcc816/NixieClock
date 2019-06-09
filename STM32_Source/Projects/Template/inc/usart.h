/**
  ******************************************************************************
  * @file    usart.h
  * @author  Lichangchun
  * @version 
  * @date    7-Jun-2019
  * @brief   stm32 串口 1 配置头文件, 启用 C 微库, 
              将格式化输入/输出(scanf/printf)重定向到 USART1 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H
#define __USART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

/* Exported functions ------------------------------------------------------- */
void USART1_Configration(uint32_t bound);
void Uart_SendData8(uint8_t Data);

#endif /* __USART_H */
