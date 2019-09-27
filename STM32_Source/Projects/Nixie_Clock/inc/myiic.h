/*******************************************************************************
* @file     --> myiic.h
* @author   --> Lichangchun
* @version  --> 1.0
* @date     --> 21-Jun-2019
* @brief    --> 
*******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_SOFT_H
#define	__I2C_SOFT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported define -----------------------------------------------------------*/
#define SCL_H         ANO_GPIO_I2C->BSRR = I2C_Pin_SCL
#define SCL_L         ANO_GPIO_I2C->BRR  = I2C_Pin_SCL 
   
#define SDA_H         ANO_GPIO_I2C->BSRR = I2C_Pin_SDA//BSRR:端口位设置/清除寄存器
#define SDA_L         ANO_GPIO_I2C->BRR  = I2C_Pin_SDA//BRR:端口位清除寄存器

#define SCL_read      ANO_GPIO_I2C->IDR  & I2C_Pin_SCL //输入寄存器与相应引脚占位
#define SDA_read      ANO_GPIO_I2C->IDR  & I2C_Pin_SDA

/***************I2C GPIO定义******************/
#define ANO_GPIO_I2C	GPIOC
#define I2C_Pin_SCL	GPIO_Pin_6
#define I2C_Pin_SDA	GPIO_Pin_7
#define ANO_RCC_I2C	RCC_APB2Periph_GPIOC

/* Exported variables---------------------------------------------------------*/
extern volatile uint8_t I2C_FastMode;

/* Exported functions ------------------------------------------------------- */
void I2C_Soft_Init(void);
void I2C_Soft_SendByte(uint8_t SendByte);
uint8_t I2C_Soft_ReadByte(uint8_t);

uint8_t I2C_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data);
uint8_t I2C_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data);
uint8_t I2C_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);
uint8_t I2C_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);

#endif /* __I2C_SOFT_H */
