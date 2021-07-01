/**
  ******************************************************************************
  * @file    ws2812b.h
  * @author  Lichangchun
  * @version 1.0
  * @date    30-Jun-2021
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
#define WS2812B_NUM             8

/* Exported functions ------------------------------------------------------- */
void WS2812B_Init(void);
void WS2812B_Show(void);
uint32_t WS2812B_Color(uint8_t r, uint8_t g, uint8_t b);
void WS2812B_Clear(void);
void WS2812B_SetPixelRGB(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void WS2812B_SetPixelRGBAll(uint8_t r, uint8_t g, uint8_t b);
void WS2812B_SetPixelColor(uint16_t n, uint32_t color);

uint32_t WS2812B_Wheel(uint8_t wheelPos);
void WS2812B_ColorWipe(uint32_t c, uint8_t wait);
void WS2812B_Rainbow(uint8_t wait);
void WS2812B_RainbowCycle(uint8_t wait);
void WS2812B_TheaterChase(uint32_t c, uint8_t wait);
void WS2812B_TheaterChaseRainbow(uint8_t wait);


#endif /* __WS2812B_H */
