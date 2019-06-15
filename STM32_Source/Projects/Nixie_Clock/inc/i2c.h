/**
  ******************************************************************************
  * @file    i2c.h
  * @author  Lichangchun
  * @version 1.0
  * @date    4-Jun-2019
  * @brief   STM32F103 I2C 配置头文件
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H
#define __I2C_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported functions ------------------------------------------------------- */
void I2C2_Init(uint16_t addr, uint32_t speed);
ErrorStatus I2C2_BufferRead(uint8_t *pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress);
uint8_t I2C2_ReadByte(uint8_t SlaveAddress);
ErrorStatus I2C2_BufferWrite(uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress);
void I2C2_WriteByte(uint8_t SlaveAddress, uint8_t data);

#endif /* __I2C_H */
