/*******************************************************************************
* @file     --> display.c
* @author   --> Lichangchun
* @version  --> 1.0
* @date     --> 6-Oct-2019
* @brief    --> 各种显示功能的实现
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "display.h"
#include "hv57708.h"
#include "neon.h"       // 氖泡驱动

/* Private variables ---------------------------------------------------------*/
uint8_t second_previous;    // 前一秒读数
uint8_t minute_previous;    // 前一分读数
uint8_t dis_data[6];        // 用于显示的数据暂存区

/*******************************************************************************
* @brief    --> 初始化
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Display_Init(void)
{
  Neon_Init();
  HV57708_Init();
  HV57708_TubePower(ENABLE);
}

/*******************************************************************************
* @brief    --> 时间显示, 时分秒
* @param    --> time - 指向要显示时间的结构体
* @retval   --> None
*******************************************************************************/
void Time_Display(Time_TypeDef *time)
{
  dis_data[5] = time->second % 10;
  dis_data[4] = time->second / 10;
  dis_data[3] = time->minute % 10;
  dis_data[2] = time->minute / 10;
  dis_data[1] = time->hour % 10;
  dis_data[0] = time->hour / 10;
  
  /* 每秒闪烁冒号 */
  if (dis_data[5] != second_previous)
  {
    second_previous = dis_data[5];
    Neon_FlipAll();
  }
  
  if (dis_data[2] != minute_previous)
  {
    /* 每 10 分钟阴极保护 */
    minute_previous = dis_data[2];
    HV57708_Protection();
  }
  else
    HV57708_Display(dis_data);
}

/*******************************************************************************
* @brief    --> 日期显示, 年月日
* @param    --> time - 指向要显示时间的结构体
* @retval   --> None
*******************************************************************************/
void Date_Display(Time_TypeDef *time)
{
  dis_data[5] = time->date % 10;
  dis_data[4] = time->date / 10;
  dis_data[3] = time->month % 10;
  dis_data[2] = time->month / 10;
  dis_data[1] = time->year % 10;
  dis_data[0] = time->year / 10;
  
  /* 不显示冒号 */
  Neon_AllOff();
  
  HV57708_Display(dis_data);
}

/*******************************************************************************
* @brief    --> 温湿度显示, 显示成 4 位小数
* @param    --> value - 要显示的值
* @retval   --> None
*******************************************************************************/
void TempOrHumi_Display(float value)
{
  uint16_t data = value * 100;
  
  dis_data[0] = 11; // 最高两位不显示 (大于 10 的数字不会显示)
  dis_data[1] = 11;
  
  dis_data[2] = data / 1000;
  data = data % 1000;
  dis_data[3] = data / 100;
  data = data % 100;
  dis_data[4] = data / 10;
  data = data % 10;
  dis_data[5] = data;
  
  HV57708_Display(dis_data);
}
