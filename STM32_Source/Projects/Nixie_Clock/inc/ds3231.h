/**
  ******************************************************************************
  * @file    ds3231.h
  * @author  Lichangchun
  * @version 1.0
  * @date    5-Jun-2019
  * @brief   DS3231 高精度实时时钟驱动头文件, I2C
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DS3231_H
#define __DS3231_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
typedef struct 
{
  uint8_t year;
  uint8_t month;
  uint8_t date;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} DS3231_TimeTypeDef;

typedef struct 
{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} DS3231_ClockTypeDef;

typedef struct 
{
  uint8_t year;
  uint8_t month;
  uint8_t date;
  uint8_t day; // 星期和日期是联动的
} DS3231_DateTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* DS3231 的 I2C 地址 */
#define DS3231_ADDRESS      0x68

/* 闹钟屏蔽位 */
#define  OncePerSecond          0x0F // 每秒
#define  SecondMatch            0x0E // 秒
#define  MinuteSecond           0x0C // 分-秒
#define  HourMinuteSecond       0x08 // 时-分-秒
#define  DateHourMinuteSecond   0x00 // 日期-时-分-秒
#define  DayHourMinuteSecond    0x10 // 星期-时-分-秒
#define  OncePerMinute        0x07 // 每分
#define  MinuteMatch          0x06 // 分
#define  HourMinute           0x04 // 时-分
#define  DateHourMinute       0x08 // 日期-时-分
#define  DayHourMinute        0x00 // 星期-时-分

/* Exported functions ------------------------------------------------------- */
void DS3231_GetTime(DS3231_TimeTypeDef *time);
void DS3231_GetClock(DS3231_ClockTypeDef *clock);
void DS3231_GetDate(DS3231_DateTypeDef *date);

void DS3231_SetTime(DS3231_TimeTypeDef *time);
void DS3231_SetClock(DS3231_ClockTypeDef *clock);
void DS3231_SetDate(DS3231_DateTypeDef *date);

void DS3231_SetAlarm1(uint8_t mode, DS3231_TimeTypeDef *time);
void DS3231_SetAlarm2(uint8_t mode, DS3231_TimeTypeDef *time);

void DS3231_TurnOnAlarm(uint8_t alarm);
void DS3231_TurnOffAlarm(uint8_t alarm);

FunctionalState DS3231_CheckAlarmEnabled(uint8_t alarm);
FunctionalState DS3231_CheckIfAlarm(uint8_t alarm);

float DS3231_GetTemperature(void);

#endif /* __DS3231_H */
