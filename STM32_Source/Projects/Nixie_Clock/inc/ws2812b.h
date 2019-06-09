/**
  ******************************************************************************
  * @file    ws2812b.h
  * @author  Lichangchun
  * @version 0.1
  * @date    2-Jun-2019
  * @brief   
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WS2812B_H
#define __WS2812B_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
// 灯珠数量定义
#define RGB_LED_NUM             4

/* Exported functions ------------------------------------------------------- */
void RGB_LED_Init(void);
void RGB_LED_Show(void);
uint32_t RGB_LED_Color(uint8_t r, uint8_t g, uint8_t b);
void RGB_LED_Clear(void);
void RGB_LED_SetPixelRGB(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void RGB_LED_SetPixelColor(uint16_t n, uint32_t color);

uint32_t RGB_LED_Wheel(uint8_t wheelPos);
void RGB_LED_ColorWipe(uint32_t c, uint8_t wait);
void RGB_LED_Rainbow(uint8_t wait);
void RGB_LED_RainbowCycle(uint8_t wait);
void RGB_LED_TheaterChase(uint32_t c, uint8_t wait);


#endif /* __WS2812B_H */
