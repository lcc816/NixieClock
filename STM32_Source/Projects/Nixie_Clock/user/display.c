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
* @brief    --> 时钟显示, 时分秒
* @param    --> clock - 指向要显示时间的结构体
* @retval   --> None
*******************************************************************************/
void Clock_Display(DS3231_ClockTypeDef *clock)
{
    static uint8_t second_previous;    // 前一秒读数
    static uint8_t minute_previous;    // 前一分读数
    uint8_t        dis_data[6];        // 用于显示的数据暂存区

    dis_data[5] = clock->second % 10;
    dis_data[4] = clock->second / 10;
    dis_data[3] = clock->minute % 10;
    dis_data[2] = clock->minute / 10;
    dis_data[1] = clock->hour % 10;
    dis_data[0] = clock->hour / 10;

    HV57708_Display(dis_data);

    /* 每秒闪烁冒号 */
    if (dis_data[5] != second_previous)
    {
        second_previous = dis_data[5];
        // Neon_FlipAll();
    }

    if (dis_data[2] != minute_previous)
    {
        /* 每 10 分钟阴极保护 */
        minute_previous = dis_data[2];
        HV57708_Protection();
    }
}

/*******************************************************************************
* @brief    --> 显示时钟, 但不用闪烁冒号和阴极保护
* @param    --> clock - 指向要显示时间的结构体
* @retval   --> None
*******************************************************************************/
void Clock_DisplayNoBlink(DS3231_ClockTypeDef *clock)
{
    uint8_t        dis_data[6];        // 用于显示的数据暂存区

    dis_data[5] = clock->second % 10;
    dis_data[4] = clock->second / 10;
    dis_data[3] = clock->minute % 10;
    dis_data[2] = clock->minute / 10;
    dis_data[1] = clock->hour % 10;
    dis_data[0] = clock->hour / 10;

    HV57708_Display(dis_data);

    Neon_AllOn();
}

/*******************************************************************************
* @brief    --> 日期显示, 年月日
* @param    --> date - 指向要显示日期的结构体
* @retval   --> None
*******************************************************************************/
void Date_Display(DS3231_DateTypeDef *date)
{
    uint8_t dis_data[6];

    dis_data[5] = date->date % 10;
    dis_data[4] = date->date / 10;
    dis_data[3] = date->month % 10;
    dis_data[2] = date->month / 10;
    dis_data[1] = date->year % 10;
    dis_data[0] = date->year / 10;

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
    uint8_t   dis_data[6];
    uint16_t  data = (uint16_t) value * 100;

    dis_data[0] = 11; // 最高两位不显示 (大于 10 的数字不会显示)
    dis_data[1] = 11;

    dis_data[2] = data / 1000;
    data = data % 1000;
    dis_data[3] = data / 100;
    data = data % 100;
    dis_data[4] = data / 10;
    data = data % 10;
    dis_data[5] = data;

    Neon_AllOff();
    HV57708_Display(dis_data);
    Neon_On(DOT4_BIT);
}
