/*******************************************************************************
* @file     --> i2c_soft.h
* @author   --> Lichangchun
* @version  --> 1.0
* @date     --> 3-Oct-2019
* @brief    --> 使用 GPIO 模拟 I2C 时序的头文件
*******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_SOFT_H
#define	__I2C_SOFT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported define -----------------------------------------------------------*/
/* I2C GPIO 引脚定义 */
#define ANO_GPIO_I2C	GPIOC
#define I2C_Pin_SCL	  GPIO_Pin_6
#define I2C_Pin_SDA	  GPIO_Pin_7
#define ANO_RCC_I2C	  RCC_APB2Periph_GPIOC

/* Exported type -------------------------------------------------------------*/
/* 错误码 */
typedef enum{
  NO_ERROR       = 0x00, // no error
  ACK_ERROR      = 0x01, // no acknowledgment error
  CHECKSUM_ERROR = 0x02, // checksum mismatch error
  TIMEOUT_ERROR  = 0x04, // timeout error
  PARM_ERROR     = 0x80, // parameter out of range error
}etError;

/* I2C 应答状态 */
typedef enum{
  ACK  = 0,
  NACK = 1,
}etI2cAck;

/* Exported functions ------------------------------------------------------- */
/* 时序相关 */
void I2c_StartCondition(void);
void I2c_StopCondition(void);

/* 通用函数 */
void I2c_Init(void);
etError I2c_WriteByte(uint8_t txByte);
etError I2c_ReadByte(uint8_t *rxByte, etI2cAck ack, uint8_t timeout);

/* 读写从机寄存器 */
etError I2c_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data);
etError I2c_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data);
etError I2c_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);
etError I2c_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);

#endif /* __I2C_SOFT_H */
