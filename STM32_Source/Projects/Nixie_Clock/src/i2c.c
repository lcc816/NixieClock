/**
******************************************************************************
* @file    i2c.c
* @author  Lichangchun
* @version 1.0
* @date    4-Jun-2019
* @brief   STM32F103 I2C 配置文件
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* Private define ------------------------------------------------------------*/
/* I2C START mask */
#define CR1_START_Set           ((uint16_t)0x0100)
#define CR1_START_Reset         ((uint16_t)0xFEFF)

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C POS mask */
#define CR1_POS_Set           ((uint16_t)0x0800)
#define CR1_POS_Reset         ((uint16_t)0xF7FF)

/*******************************************************************************
* @brief  初始化 I2C1 外设: I2Cx, GPIO
* @param  addr 作为从机时的地址
          speed 速率
* @retval None
*******************************************************************************/
void I2C1_Init(uint16_t addr, uint32_t speed)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;
  
  /* GPIO 时钟使能 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  /* I2C1 时钟使能 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  /* I2C1 SDA 和 SCL 配置: PB6--I2C1_SCL, PB7--I2C1_SDA */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 复用开漏输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* I2C 时钟复位 */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
  /* I2C1 从复位状态释放 */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
  
  /* I2C1 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = addr;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = speed;
  I2C_Init(I2C1, &I2C_InitStructure);
  
  I2C_Cmd(I2C1, ENABLE);
}

/*******************************************************************************
* @brief  I2C1 主机读取 1 个字节
* @param  addr 从机地址
* @retval 读取到的数据
*******************************************************************************/
uint8_t I2C1_ReadByte(uint8_t SlaveAddress)
{
  uint8_t value = 0;
  I2C1_BufferRead(&value, 1, SlaveAddress);
  
  return value;
}

/*******************************************************************************
* @brief  I2C 从从机读取数据到缓冲区
* @param  pBuffer 缓冲区地址
          NunByteToRead 要读取的字节数
          SlaveAddress 从机地址
* @retval 读取成功返回 SUCCESS, 失败返回 ERROR
*******************************************************************************/
ErrorStatus I2C1_BufferRead(uint8_t *pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress)
{
  __IO uint32_t temp = 0;
  __IO uint16_t TimeOut = 0;
  
  if (NumByteToRead == 1)
  {
    /* 发送 START 条件 */ 
    I2C1->CR1 |= CR1_START_Set;
    /* 等待 SB 标志被设置: EV5 */
    TimeOut = 0xFFFF;
    while ((I2C1->SR1 & 0x0001) != 0x0001)
    {
//      if (TimeOut-- == 0)
//      {
//        return ERROR;
//      }
    }
    /* 发送从地址, 最低位置 1 表示读 */
    SlaveAddress |= (uint8_t)0x01;
    I2C1->DR = SlaveAddress;
    /* 等待直至 ADDR 被设置位, 然后软件清除 ACK 位, 清除 ADDR 位并紧接着软件
    设置 STOP 位. EV6 软件序列必须在当前字节传输结束前完成: EV6 */
    /* 等待 ADDR 被置位 */
    TimeOut = 0xFFFF;
    while ((I2C1->SR1 & 0x0002) != 0x0002)
    {
//      if (TimeOut-- == 0)
//      {
//        return ERROR;
//      }
    }
    /* 清除 ACK 位 */
    I2C1->CR1 &= CR1_ACK_Reset;
    /* 清除 ADDR 和置位 STOP 时应禁止所有 IRQ, 使 EV6 能在当前字节传输结束前完成 */
    __disable_irq();
    /* 通过读 SR2 清除 ADDR 标志 */
    temp = I2C1->SR2;
    /* 设置 STOP 位 */
    I2C1->CR1 |= CR1_STOP_Set;
    /* 使能 IRQ */
    __enable_irq();
    /* 等待直至 DR 寄存器收到数据(RxNE = 1): EV7 */
    while ((I2C1->SR1 & 0x0040) != 0x0040);
    /* 读取数据 */
    *pBuffer = I2C1->DR;
    /* 确保写 CR1 寄存器前, STOP 位被硬件清除 */
    while ((I2C1->CR1 & 0x0200) == 0x0200);
    /* 使能应答, 准备下一次接收 */
    I2C1->CR1 |= CR1_ACK_Set;
  }
  
  else if (NumByteToRead == 2)
  {
    /* 设置 POS 位 */
    I2C1->CR1 |= CR1_POS_Set;
    TimeOut = 0xFFFF;
    /* 发送 START 条件 */ 
    I2C1->CR1 |= CR1_START_Set;
    /* 等待 SB 标志被设置: EV5 */
    while ((I2C1->SR1 & 0x0001) != 0x0001)
    {
//      if (TimeOut-- == 0)
//      {
//        return ERROR;
//      }
    }
    /* 发送从地址, 最低位置 1 表示读 */
    SlaveAddress |= (uint8_t)0x01;
    I2C1->DR = SlaveAddress;
    /* 等待 ADDR 被置位: EV6 */
    TimeOut = 0xFFFF;
    while ((I2C1->SR1 & 0x0002) != 0x0002)
    {
//      if (TimeOut-- == 0)
//      {
//        return ERROR;
//      }
    }
    /* EV6_1: 在清除 ADDR 位后应当立即禁止应答, 所以禁止 IRQ */
    __disable_irq();
    /* 通过读 SR2 清除 ADDR 标志 */
    temp = I2C1->SR2;
    /* 清除 ACK 位 */
    I2C1->CR1 &= CR1_ACK_Reset;
    /* 使能 IRQ */
    __enable_irq();
    /* 等待至 BIT 位被设置 */
    while ((I2C1->SR1 & 0x00004) != 0x000004);
    /* 产生 STOP 条件时禁止 IRQ */
    __disable_irq();
    /* 产生 STOP */
    I2C1->CR1 |= CR1_STOP_Set;
    /* 读取数据 */
    *pBuffer = I2C1->DR;
    /* 使能 IRQs */
    __enable_irq();
    /* 读取第二字节 */
    pBuffer++;
    *pBuffer = I2C1->DR;
    /* 确保写 CR1 寄存器前, STOP 位被硬件清除 */
    while ((I2C1->CR1 & 0x0200) == 0x0200);
    /* 使能应答, 准备下一次接收 */
    I2C1->CR1 |= CR1_ACK_Set;
    /* 清除 POS 位 */
    I2C1->CR1 &= CR1_POS_Reset;
  }
  
  else
  {
    TimeOut = 0xFFFF;
    /* 发送 START 条件 */
    I2C1->CR1 |= CR1_START_Set;
    /* 等待 SB 位被设置: EV5 */
    while ((I2C1->SR1 & 0x0001) != 0x0001)
    {
//      if (TimeOut-- == 0)
//        return ERROR;
    }
    TimeOut = 0xFFFF;
    /* 发送从地址, 最低位置 1 表示读 */
    SlaveAddress |= (uint8_t)0x01;
    I2C1->DR = SlaveAddress;
    /* 等待 ADDR 被置位: EV6 */
    while ((I2C1->SR1 & 0x0002) != 0x0002)
    {
//      if (TimeOut-- == 0)
//        return ERROR;
    }
    /* 通过读 SR2 清除 ADDR 位 */
    temp = I2C1->SR2;
    /* 循环读走数据 */
    while (NumByteToRead)
    {
      /* Receive bytes from first byte until byte N-3 */
      if (NumByteToRead != 3)
      {
        /* 轮询 BTF以接收数据, 因为在当前字节传输结束前不能保证 EV7 软件序列是可控的 */
        while ((I2C1->SR1 & 0x00004) != 0x000004);
        /* 读取数据 */
        *pBuffer = I2C1->DR;
        /* */
        pBuffer++;
        NumByteToRead--;
      }
      
      /* 还需读取 3 个字节: data N-2, data N-1, Data N */
      if (NumByteToRead == 3)
      {
        /* 等待 BTF 被置位: Data N-2 i在 DR, data N-1 还在移位寄存器中 */
        while ((I2C1->SR1 & 0x00004) != 0x000004);
        /* 清除 ACK */
        I2C1->CR1 &= CR1_ACK_Reset;
        
        /* 读数据和发送 STOP 条件时禁止 IRQs */
        __disable_irq();
        /* 读取 Data N-2 */
        *pBuffer = I2C1->DR;
        /* Increment */
        pBuffer++;
        /* 置位 STOP */
        I2C1->CR1 |= CR1_STOP_Set;
        /* 读取 DataN-1 */
        *pBuffer = I2C1->DR;
        /* 使能 IRQs */
        __enable_irq();
        pBuffer++;
        /* 等待 RXNE 置位 (DR 包含最后一条数据) */
        while ((I2C1->SR1 & 0x00040) != 0x000040);
        /* 读取 DataN */
        *pBuffer = I2C1->DR;

        NumByteToRead = 0;
      }
    }
    /* 保证在读 CR1前 STOP 位被硬件清零 */
    while ((I2C1->CR1&0x200) == 0x200);
    /* 使能应答准备下次接收 */
    I2C1->CR1 |= CR1_ACK_Set;
  }
  return SUCCESS;
}

/*******************************************************************************
* @brief  I2C1 主机发送缓冲区数据到从机
* @param  pBuffer 缓冲区地址
          NunByteToRead 要发送的字节数
          SlaveAddress 从机地址
* @retval 读取成功返回 SUCCESS, 失败返回 ERROR
*******************************************************************************/
ErrorStatus I2C1_BufferWrite(uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress)
{
  __IO uint16_t Timeout = 0;
  __IO uint32_t temp = 0;
  /* 使能 Error 中断 */
  I2C1->CR2 |= I2C_IT_ERR;
  
  Timeout = 0xFFFF;
  /* 发送 START 条件 */
  I2C1->CR1 |= CR1_START_Set;
  /* 等待 SB 标志被置位: EV5 */
  while ((I2C1->SR1 & 0x0001) != 0x0001)
  {
//    if (Timeout-- == 0)
//      return ERROR;
  }
  
  /* 发送从机地址, 最低位清 0 表示写 */
  SlaveAddress &= (uint8_t)0xFE;
  I2C1->DR = SlaveAddress;
  Timeout = 0xFFFF;
  /* 等待 ADDR 置位: EV6 */
  while ((I2C1->SR1 & 0x0002) != 0x0002)
  {
//    if (Timeout-- == 0)
//      return ERROR;
  }
  
  /* 通过读 SR2 清除 ADDR 位 */
  temp = I2C1->SR2;
  /* 将第一字节写入 DR (EV8_1) */
  I2C1->DR = *pBuffer;
  /* 递增缓冲区地址 */
  pBuffer++;
  /* 递减要发送的字节数 */
  NumByteToWrite--;
  /* While there is data to be written */
  while (NumByteToWrite--)
  {
    /* 轮询检查 BTF 位, 等待字节发送完成 */
    while ((I2C1->SR1 & 0x0004) != 0x0004);
    /* 发送当前字节 */
    I2C1->DR = *pBuffer;
    /* 指向下一个要发送的字节 */
    pBuffer++;
  }
  /* EV8_2: 发送 STOP 条件前等待 BTF */
  while ((I2C1->SR1 & 0x0004) != 0x0004);
  /* 发送 STOP 条件 */
  I2C1->CR1 |= CR1_STOP_Set;
  /* 确保 STOP 位被硬件清零 */
  while ((I2C1->CR1 & 0x200) == 0x200);
  
  return SUCCESS;
}

/*******************************************************************************
* @brief  I2C1 发送 1 个字节
* @param  addr 从机地址
          data 要发送的数据
* @retval None.
*******************************************************************************/
void I2C1_WriteByte(uint8_t SlaveAddress, uint8_t data)
{
  I2C1_BufferWrite(&data, 1, SlaveAddress);
}
