/*******************************************************************************
* @file     --> i2c_soft.c
* @author   --> Lichangchun
* @version  --> 1.0
* @date     --> 30-Sept-2019
* @brief    --> 软 I2C 驱动
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c_soft.h"
#include "delay.h"

/* Private define ------------------------------------------------------------*/
/* I2C 电平定义 */
#define SDA_LOW()     ANO_GPIO_I2C->BRR  = I2C_Pin_SDA // 低电平
#define SDA_OPEN()    ANO_GPIO_I2C->BSRR = I2C_Pin_SDA // 开漏
#define SDA_READ      (ANO_GPIO_I2C->IDR  & I2C_Pin_SDA) // 读 SDA

#define SCL_LOW()     ANO_GPIO_I2C->BRR  = I2C_Pin_SCL // 低电平
#define SCL_OPEN()    ANO_GPIO_I2C->BSRR = I2C_Pin_SCL // 开漏
#define SCL_READ      (ANO_GPIO_I2C->IDR  & I2C_Pin_SCL) // 读 SCL

#define DelayMicroSeconds(n)    delay_us(n) // 延时 n us

/* Private functions ---------------------------------------------------------*/
static etError I2c_WaitWhileClockStreching(uint8_t timeout);

/*******************************************************************************
* @brief    --> I2C 初始化函数
* @param    --> None
* @retval   --> None
*******************************************************************************/
void I2c_Init(void) 
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
* @retval   --> None
*******************************************************************************/
void I2c_StartCondition(void)
{
  SDA_OPEN();
  DelayMicroSeconds(1);
  SCL_OPEN();
  DelayMicroSeconds(1);
  SDA_LOW();
  DelayMicroSeconds(10);  // hold time start condition (t_HD;STA)
  SCL_LOW();
  DelayMicroSeconds(10);
}

/*******************************************************************************
* @brief    --> 产生 I2C 停止信号
* @param    --> None
* @retval   --> None
*******************************************************************************/
void I2c_StopCondition(void)
{
  SCL_LOW();
  DelayMicroSeconds(1);
  SDA_LOW();
  DelayMicroSeconds(1);
  SCL_OPEN();
  DelayMicroSeconds(10);  // set-up time stop condition (t_SU;STO)
  SDA_OPEN();
  DelayMicroSeconds(10);
}

/*******************************************************************************
* @brief    --> I2C 发送一个字节, 数据从高位到低位
* @param    --> txByte    - 被发送的字节
* @retval   --> ACK_ERROR = 传感器无应答
*               NO_ERROR  = 无错误
*******************************************************************************/
etError I2c_WriteByte(uint8_t txByte)
{
  etError error = NO_ERROR;
  uint8_t     mask;
  for(mask = 0x80; mask > 0; mask >>= 1)// shift bit for masking (8 times)
  {
    if((mask & txByte) == 0) SDA_LOW(); // masking txByte, write bit to SDA-Line
    else                     SDA_OPEN();
    DelayMicroSeconds(1);               // data set-up time (t_SU;DAT)
    SCL_OPEN();                         // generate clock pulse on SCL
    DelayMicroSeconds(5);               // SCL high time (t_HIGH)
    SCL_LOW();
    DelayMicroSeconds(1);               // data hold time(t_HD;DAT)
  }
  SDA_OPEN();                           // release SDA-line
  SCL_OPEN();                           // clk #9 for ack
  DelayMicroSeconds(1);                 // data set-up time (t_SU;DAT)
  if(SDA_READ) error = ACK_ERROR;       // check ack from i2c slave
  SCL_LOW();
  DelayMicroSeconds(20);                // wait to see byte package on scope
  return error;                         // return error code
}

/*******************************************************************************
* @brief    --> I2C 读取一个字节, 数据从高位到低位
* @param    --> rxByte        - 指向读取数据的指针
*               ack           - 发送 ACK or NACK
*               timeout       - 超时时间 (us)
* @retval   --> TIMEOUT_ERROR = 超时错误
*               NO_ERROR      = 无错误
*******************************************************************************/
etError I2c_ReadByte(uint8_t *rxByte, etI2cAck ack, uint8_t timeout)
{
  etError error = NO_ERROR;
  uint8_t mask;
  *rxByte = 0x00;
  SDA_OPEN();                            // release SDA-line
  for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
  { 
    SCL_OPEN();                          // start clock on SCL-line
    DelayMicroSeconds(1);                // clock set-up time (t_SU;CLK)
    error = I2c_WaitWhileClockStreching(timeout);// wait while clock streching
    DelayMicroSeconds(3);                // SCL high time (t_HIGH)
    if(SDA_READ) *rxByte |= mask;        // read bit
    SCL_LOW();
    DelayMicroSeconds(1);                // data hold time(t_HD;DAT)
  }
  if(ack == ACK) SDA_LOW();              // send acknowledge if necessary
  else           SDA_OPEN();
  DelayMicroSeconds(1);                  // data set-up time (t_SU;DAT)
  SCL_OPEN();                            // clk #9 for ack
  DelayMicroSeconds(5);                  // SCL high time (t_HIGH)
  SCL_LOW();
  SDA_OPEN();                            // release SDA-line
  DelayMicroSeconds(20);                 // wait to see byte package on scope
  
  return error;                          // return with no error
}

/*******************************************************************************
* @brief    --> I2C 写一个字节数据到从机的指定地址
* @param    --> SlaveAddress  - 从机地址
*               REG_Address   - 寄存器地址
*               REG_data      - 字节数据
* @retval   --> ACK_ERROR     = 应答错误
*               NO_ERROR      = 无错误
*******************************************************************************/
etError I2c_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
  etError error;
  
  I2c_StartCondition();
  
  error = I2c_WriteByte(SlaveAddress << 1);
  error |= I2c_WriteByte(REG_Address);
  error |= I2c_WriteByte(REG_data);
  
  I2c_StopCondition();
  
  return error;
}

/*******************************************************************************
* @brief    --> I2C 从从机的指定地址读取一个字节
* @param    --> SlaveAddress  - 从机地址
*               REG_Address   - 寄存器地址
*               REG_data      - 指向读取字节的指针
* @retval   --> ACK_ERROR     = 应答错误
*               TIMEOUT_ERROR = 超时错误
*               NO_ERROR      = 无错误
*******************************************************************************/
etError I2c_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data)
{
  etError error;
  
  I2c_StartCondition();
  
  error = I2c_WriteByte(SlaveAddress << 1);
  error |= I2c_WriteByte(REG_Address);
  
  if (error != NO_ERROR) {I2c_StopCondition(); return error;}
  
  I2c_StartCondition();
  
  error = I2c_WriteByte(SlaveAddress << 1 | 0x01);
  if (error == NO_ERROR) error = I2c_ReadByte(REG_data, NACK, 0);
  
  I2c_StopCondition();
  
  return error;
}

/*******************************************************************************
* @brief    --> I2C 向从机的指定地址写入 n 个字节
* @param    --> SlaveAddress  - 从机地址
*               REG_Address   - 寄存器地址
*               len           - 字节数据
*               buf           - 发送数据缓冲区
* @retval   --> ACK_ERROR     = 应答错误
*               TIMEOUT_ERROR = 超时错误
*               NO_ERROR      = 无错误
*******************************************************************************/
etError I2c_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
  etError error;
  
  I2c_StartCondition();
  
  error = I2c_WriteByte(SlaveAddress << 1);
  error |= I2c_WriteByte(REG_Address);
  
  if (error != NO_ERROR) {I2c_StopCondition(); return error;}
  
  while (len--)
  {
    error = I2c_WriteByte(*buf++);
    if (error != NO_ERROR)
      break;
  }
  
  I2c_StopCondition();
  
  return error;
}

/*******************************************************************************
* @brief    --> I2C 从从机的指定地址读取 n 个字节
* @param    --> SlaveAddress  - 从机地址
*               REG_Address   - 寄存器地址
*               len           - 字节数据
*               buf           - 接收数据缓冲区
* @retval   --> ACK_ERROR     = 应答错误
*               TIMEOUT_ERROR = 超时错误
*               NO_ERROR      = 无错误
*******************************************************************************/
etError I2c_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
  etError error;
  
  I2c_StartCondition();
  
  error = I2c_WriteByte(SlaveAddress << 1);
  error |= I2c_WriteByte(REG_Address);
  
  if (error != NO_ERROR) {I2c_StopCondition(); return error;}
  
  I2c_StartCondition();
  
  error = I2c_WriteByte(SlaveAddress << 1 | 0x01);
  
  while (len-- > 1)
  {
    if (error == NO_ERROR) error = I2c_ReadByte(buf++, ACK, 0);
    else break;
  }
  
  if (error != NO_ERROR) {I2c_StopCondition(); return error;}
  
  error = I2c_ReadByte(buf++, NACK, 0);
  
  return error;
}

//-----------------------------------------------------------------------------
static etError I2c_WaitWhileClockStreching(uint8_t timeout)
{
  etError error = NO_ERROR;
  
  while(SCL_READ == 0)
  {
    if(timeout-- == 0) return TIMEOUT_ERROR;
    DelayMicroSeconds(1000);
  }
  
  return error;
}
