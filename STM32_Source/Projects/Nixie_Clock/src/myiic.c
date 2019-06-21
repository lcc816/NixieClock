/*******************************************************************************
* @file     --> myiic.c
* @author   --> Lichangchun
* @version  --> 1.0
* @date     --> 21-Jun-2019
* @brief    --> 
*******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "myiic.h"

/* Private variables ---------------------------------------------------------*/
volatile uint8_t I2C_FastMode;

/* Private functions ---------------------------------------------------------*/

void I2C_Soft_delay(void)
{ 
	unsigned char a=2;
  while(a--);
  //__nop();__nop();__nop();
	//__nop();__nop();__nop();
	//__nop();__nop();__nop();
	
	if(!I2C_FastMode)  //如果不是快速模式
	{
		uint8_t i = 15;
		while(i--);
	}
}

/*******************************************************************************
* @brief    --> I2C 初始化函数
* @param    --> None
* @retval   --> None
*******************************************************************************/
void I2C_Soft_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd(ANO_RCC_I2C, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  I2C_Pin_SCL | I2C_Pin_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ANO_GPIO_I2C, &GPIO_InitStructure);		
}

/*******************************************************************************
* @brief    --> 产生 I2C 起始信号
* @param    --> None
* @retval   --> 1 为成功, 0 为出错
*******************************************************************************/
uint8_t I2C_Soft_Start(void)
{
  SDA_H;
  SCL_H;
  I2C_Soft_delay();
  if(!SDA_read) return 0;	// SDA 线为低电平则总线忙, 退出
  SDA_L;
  I2C_Soft_delay();
  if(SDA_read) return 0;	// SDA 线为高电平则总线出错, 退出
  SDA_L;
  I2C_Soft_delay();
  return 1;
}

/*******************************************************************************
* @brief    --> 产生 I2C 停止信号
* @param    --> None
* @retval   --> None
*******************************************************************************/
void I2C_Soft_Stop(void)
{
  SCL_L;
  I2C_Soft_delay();
  SDA_L;
  I2C_Soft_delay();
  SCL_H;
  I2C_Soft_delay();
  SDA_H;
  I2C_Soft_delay();
}

/*******************************************************************************
* @brief    --> 产生 I2C 应答信号
* @param    --> None
* @retval   --> None
*******************************************************************************/
void I2C_Soft_Ack(void)
{
  SCL_L;
  I2C_Soft_delay();
  SDA_L;
  I2C_Soft_delay();
  SCL_H;
  I2C_Soft_delay();
  SCL_L;
  I2C_Soft_delay();
}

/*******************************************************************************
* @brief    --> 产生 I2C 无应答信号
* @param    --> None
* @retval   --> None
*******************************************************************************/
void I2C_Soft_NoAck(void)
{
  SCL_L;
  I2C_Soft_delay();
  SDA_H;
  I2C_Soft_delay();
  SCL_H;
  I2C_Soft_delay();
  SCL_L;
  I2C_Soft_delay();
}

/*******************************************************************************
* @brief    --> 等待应答信号到来
* @param    --> None
* @retval   --> None
*******************************************************************************/
uint8_t I2C_Soft_WaitAck(void)
{
  uint8_t ErrTime = 0;
  SCL_L;
  I2C_Soft_delay();
  SDA_H;			
  I2C_Soft_delay();
  SCL_H;
  I2C_Soft_delay();
  while(SDA_read)
  {
    ErrTime++;
    if(ErrTime > 50)
    {
      I2C_Soft_Stop();
      return 1;
    }
  }
  SCL_L;
  I2C_Soft_delay();
  return 0;
}

/*******************************************************************************
* @brief    --> I2C 发送一个字节, 数据从高位到低位
* @param    --> SendByte: 被发送的字节
* @retval   --> None
*******************************************************************************/
void I2C_Soft_SendByte(uint8_t SendByte)
{
  uint8_t i = 8;
  while(i--)
  {
    SCL_L;  //拉低时钟开始数据传输
    I2C_Soft_delay();
  if(SendByte & 0x80)
    SDA_H;  
  else 
    SDA_L;   
    SendByte <<= 1;
    I2C_Soft_delay();
    SCL_H;
    I2C_Soft_delay();
  }
  SCL_L;
}

/*******************************************************************************
* @brief    --> I2C 读取一个字节, 数据从高位到低位
* @param    --> ack: =1时, 发送 ACK; =0, 发送NACK
* @retval   --> 返回读取的数据
*******************************************************************************/
uint8_t I2C_Soft_ReadByte(uint8_t ack)
{ 
  uint8_t i = 8;
  uint8_t ReceiveByte = 0;

  SDA_H;				
  while(i--)
  {
    ReceiveByte <<= 1;      
    SCL_L;
    I2C_Soft_delay();
    SCL_H;
    I2C_Soft_delay();	
    if(SDA_read)
    {
      ReceiveByte |= 0x01;
    }
  }
  SCL_L;

  if (ack)
    I2C_Soft_Ack();
  else
    I2C_Soft_NoAck();  
  return ReceiveByte;
}

/*******************************************************************************
* @brief    --> I2C 写一个字节数据到从机的指定地址
* @param    --> SlaveAddress: 从机地址
*               REG_Address:  寄存器地址
*               REG_data:     字节数据
* @retval   --> None
*******************************************************************************/
uint8_t I2C_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
	I2C_Soft_Start();
	I2C_Soft_SendByte(SlaveAddress);   
	if(I2C_Soft_WaitAck())//如果无应答信号到来
	{
    I2C_Soft_Stop();
    return 1;
	}
	I2C_Soft_SendByte(REG_Address);       
	I2C_Soft_WaitAck();	
	I2C_Soft_SendByte(REG_data);
	I2C_Soft_WaitAck();   
	I2C_Soft_Stop(); 
	return 0;
}

/*******************************************************************************
* @brief    --> I2C 从从机的指定地址读取一个字节
* @param    --> SlaveAddress: 从机地址
*               REG_Address:  寄存器地址
*               REG_data:     字节数据
* @retval   --> =1, 读取失败; =0, 读取成功
*******************************************************************************/
uint8_t I2C_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data)
{      		
	I2C_Soft_Start();
	I2C_Soft_SendByte(SlaveAddress); 
	if(I2C_Soft_WaitAck())
	{
    I2C_Soft_Stop();
    return 1;
	}
	I2C_Soft_SendByte(REG_Address);     
	I2C_Soft_WaitAck();
	I2C_Soft_Start();
	I2C_Soft_SendByte(SlaveAddress + 1);
	I2C_Soft_WaitAck();
	*REG_data = I2C_Soft_ReadByte(0);
	I2C_Soft_Stop();
	return 0;
}	

/*******************************************************************************
* @brief    --> I2C 向从机的指定地址写入 n 个字节
* @param    --> SlaveAddress: 从机地址
*               REG_Address:  寄存器地址
*               len:          字节数据
*               buf:          发送数据缓冲区
* @retval   --> =1, 写入失败; =0, 写入成功
*******************************************************************************/
uint8_t I2C_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{	
	I2C_Soft_Start();
	I2C_Soft_SendByte(SlaveAddress); 
	if(I2C_Soft_WaitAck())
	{
    I2C_Soft_Stop();
    return 1;
	}
	I2C_Soft_SendByte(REG_Address); 
	I2C_Soft_WaitAck();
	while(len--) 
	{
    I2C_Soft_SendByte(*buf++); 
    I2C_Soft_WaitAck();
	}
	I2C_Soft_Stop();
	return 0;
}

/*******************************************************************************
* @brief    --> I2C 从从机的指定地址读取 n 个字节
* @param    --> SlaveAddress: 从机地址
*               REG_Address:  寄存器地址
*               len:          字节数
*               buf:          接收数据缓冲区
* @retval   --> =1, 读取失败; =0, 读取成功
*******************************************************************************/
uint8_t I2C_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{	
  I2C_Soft_Start();
  I2C_Soft_SendByte(SlaveAddress); 
  if(I2C_Soft_WaitAck())
  {
    I2C_Soft_Stop();
    return 1;
  }
  I2C_Soft_SendByte(REG_Address); 
  I2C_Soft_WaitAck();
  
  I2C_Soft_Start();
  I2C_Soft_SendByte(SlaveAddress + 1); 
  I2C_Soft_WaitAck();
  while(len) 
  {
    if(len == 1)
    {
      *buf = I2C_Soft_ReadByte(0);
    }
    else
    {
      *buf = I2C_Soft_ReadByte(1);
    }
    buf++;
    len--;
  }
  I2C_Soft_Stop();
  return 0;
}
