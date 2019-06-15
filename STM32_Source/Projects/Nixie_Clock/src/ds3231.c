/**
  ******************************************************************************
  * @file    ds3231.c
  * @author  Lichangchun
  * @version 1.0
  * @date    5-Jun-2019
  * @brief   DS3231 高精度实时时钟, I2C 驱动
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "ds3231.h"
#include "i2c.h"

/* Private functions ---------------------------------------------------------*/
uint8_t BcdToDec(uint8_t val);
uint8_t DecToBcd(uint8_t val);

uint8_t ReadControlByte(void);
void WriteControlByte(uint8_t data);

uint8_t ReadStatusByte(void);
void WriteStatusByte(uint8_t data);

/*******************************************************************************
  * @brief  获取当前时间, 统一 24 小时制
  * @param  None
  * @retval 存储当前时间的结构体
*******************************************************************************/
TimeTypeDef DS3231_GetTime(void)
{
  TimeTypeDef time;
  uint8_t buffer[7];
  
  /* 发送计时寄存器起始地址 */
  I2C2_WriteByte(DS3231_ADDRESS, (uint8_t)0x00);
  /* 连续读取 7 个字节 */
  I2C2_BufferRead(buffer, 7, DS3231_ADDRESS);
  
  time.second = BcdToDec(buffer[0]);
  time.minute = BcdToDec(buffer[1]);
  /* 处理 12/24 小时制 */
  if ((buffer[2] & 0x40) == 0x40) // 12 小时制
  {
    time.hour = BcdToDec(buffer[2] & 0x1F);
    if ((buffer[2] & 0x20) == 0x20) // PM
    {
      time.hour += 12;
    }
  }
  else
  {
    time.hour = BcdToDec(buffer[2] & 0x3F);
  }
  time.day = BcdToDec(buffer[3]);
  time.date = BcdToDec(buffer[4]);
  time.month = BcdToDec(buffer[5] & 0x7F);
  time.year = BcdToDec(buffer[6]);
  
  return time;
}

/*******************************************************************************
  * @brief  初始化 DS3231 计时寄存器
  * @param  time 要设置的时间
  * @retval None
*******************************************************************************/
void DS3231_SetTime(TimeTypeDef time)
{
  uint8_t buffer[8];
  /* buffer[0] 存放寄存器开始地址 */
  buffer[0] = 0x00;
  buffer[1] = DecToBcd(time.second);
  buffer[2] = DecToBcd(time.minute);
  buffer[3] = DecToBcd(time.hour);
  buffer[4] = DecToBcd(time.day);
  buffer[5] = DecToBcd(time.date);
  buffer[6] = DecToBcd(time.month);
  buffer[7] = DecToBcd(time.year);
  
  /* 连续写入多个字节 */
  I2C2_BufferWrite(buffer, 8, DS3231_ADDRESS);
}

/*******************************************************************************
  * @brief  设置闹钟 1, 可设置到 秒
  * @param  mode 闹钟模式
  *         time 要设置的时间
  * @retval None
*******************************************************************************/
void DS3231_SetAlarm1(uint8_t mode, TimeTypeDef time)
{
  /* 闹钟 1 有连续 4 个寄存器, buffer[0] 存放起始地址 */
  uint8_t buffer[5];
  buffer[0] = 0x07;
  buffer[1] = DecToBcd(time.second) | (mode & 0x01) << 7;
  buffer[2] = DecToBcd(time.minute) | (mode & 0x02) << 6;
  buffer[3] = DecToBcd(time.hour) | (mode & 0x04) << 5;
  /* 按星期重复 or 日期重复 */
  if ((mode & 0x10) == 0x10)
    buffer[4] = DecToBcd(time.day) | (mode & 0x08) << 4;
  else
    buffer[4] = DecToBcd(time.date) | (mode & 0x08) << 4;
  
  I2C2_BufferWrite(buffer, 5, DS3231_ADDRESS);
}

/*******************************************************************************
  * @brief  设置闹钟 2, 可设置到 分
  * @param  mode 闹钟模式
  *         time 要设置的时间
  * @retval None
*******************************************************************************/
void DS3231_SetAlarm2(uint8_t mode, TimeTypeDef time)
{
  /* 闹钟 2 有连续 4 个寄存器, buffer[0] 存放起始地址 */
  uint8_t buffer[4];
  buffer[0] = 0x0B;
  buffer[1] = DecToBcd(time.minute) | (mode & 0x01) << 7;
  buffer[2] = DecToBcd(time.hour) | (mode & 0x02) << 6;
  /* 按星期重复 or 日期重复 */
  if ((mode & 0x08) == 0x08)
    buffer[3] = DecToBcd(time.day) | (mode & 0x04) << 5;
  else
    buffer[3] = DecToBcd(time.date) | (mode & 0x04) << 5;
  
  I2C2_BufferWrite(buffer, 4, DS3231_ADDRESS);
}

/*******************************************************************************
  * @brief  打开闹钟
  * @param  alarm 闹钟 1 or 2, 缺省为 2
  * @retval None
*******************************************************************************/
void DS3231_TurnOnAlarm(uint8_t alarm)
{
  uint8_t temp = ReadControlByte();
  if (alarm == 1)
    temp |= 0x05;
  else
    temp |= 0x06;
  WriteControlByte(temp);
}

/*******************************************************************************
  * @brief  关闭闹钟
  * @param  alarm 闹钟 1 or 2, 缺省为 2
  * @retval None
*******************************************************************************/
void DS3231_TurnOffAlarm(uint8_t alarm)
{
  uint8_t temp = ReadControlByte();
  if (alarm == 1)
    temp &= 0xFE;
  else
    temp &= 0xFD;
  WriteControlByte(temp);
}

/*******************************************************************************
  * @brief  检查指定闹钟是否开启
  * @param  alarm 指定闹钟 1 or 2, 缺省为 2
  * @retval ENABLE 开启, DISABLE 未开启
*******************************************************************************/
FunctionalState DS3231_CheckAlarmEnabled(uint8_t alarm)
{
  uint8_t result = 0x00;
  uint8_t temp = ReadControlByte();
  
  if (alarm == 1)
    result = temp & 0x01;
  else
    result = temp & 0x02;
  
  if (result)
    return ENABLE;
  else
    return DISABLE;
}

/*******************************************************************************
  * @brief  检查闹钟是否响
  * @param  alarm 指定闹钟 1 or 2, 缺省为 2
  * @retval ENABLE 响, DISABLE 未响
*******************************************************************************/
FunctionalState DS3231_CheckIfAlarm(uint8_t alarm)
{
  uint8_t result = 0x00;
  uint8_t temp = ReadStatusByte();
  /* 判断哪个闹钟响 */
  if (alarm == 1)
  {
    result = temp & 0x01;
    temp &= 0xFE;
  }
  else
  {
    result = temp & 0x02;
    temp &= 0xFD;
  }
  /* 清除闹钟标志 */
  WriteStatusByte(temp);
  
  if (result)
    return ENABLE;
  else
    return DISABLE;
}

////// 内部函数 //////

// 控制寄存器 0x0E
uint8_t ReadControlByte(void)
{
  I2C2_WriteByte(DS3231_ADDRESS, 0x0E);
  return I2C2_ReadByte(DS3231_ADDRESS);
}

void WriteControlByte(uint8_t data)
{
  uint8_t  temp[2] = {0x0E, data};
  I2C2_BufferWrite(temp, 2, DS3231_ADDRESS);
}

// 状态寄存器 0x0F
uint8_t ReadStatusByte(void)
{
  I2C2_WriteByte(DS3231_ADDRESS, 0x0F);
  return I2C2_ReadByte(DS3231_ADDRESS);
}

void WriteStatusByte(uint8_t data)
{
  uint8_t  temp[2] = {0x0F, data};
  I2C2_BufferWrite(temp, 2, DS3231_ADDRESS);
}

// 二十进制转换

uint8_t BcdToDec(uint8_t val)
{
  return (val >> 4) * 10 + (val & 0x0F);
}

uint8_t DecToBcd(uint8_t val)
{
  return ((val / 10) << 4) + val % 10;
}
