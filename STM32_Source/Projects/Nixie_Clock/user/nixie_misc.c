/*
 * nixie_misc.c
 *
 *  Created on: Aug 28, 2021
 *      Author: lcc
 */

/* Includes ------------------------------------------------------------------*/
#include "nixie_misc.h"

/*******************************************************************************
 * @brief   判断是否是闰年
 * @param   year 年
 * @retval  是则返回 1，不是返回 0
 *******************************************************************************/
int is_leap_year(uint16_t year)
{
    if ((year % 100 != 0 && year % 4 == 0) || (year % 100 == 0 && year % 400 == 0))
    {
        return 1;
    }

    return 0;
}

/*******************************************************************************
 * @brief   将日期换算为星期
 * @param   year 年
 * @param   month 月
 * @param   date 日
 * @param   day 指向星期几的指针
 * @retval  计算成功返回 0
 *******************************************************************************/
int date2day(uint16_t year, uint8_t month, uint8_t date, uint8_t *day)
{
    uint8_t century;
    uint8_t _year;
    uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (day == NULL)
        return -1;

    if (month < 1 || month > 12)
        return -1;

    if (is_leap_year(year) && month == 2)
    {
        if (date < 1 || date > 29)
            return -1;
    }
    else
    {
        if (date < 1 || date > days[month - 1])
            return -1;
    }

    /* 1、2 月份看作上一年的 13、14 月份 */
    if (month == 1 || month == 2)
    {
        month += 12;
        year -= 1;
    }
    century = year / 100;
    _year = year % 100;

    *day = (_year+ _year / 4 + century / 4 - 2 * century + 26 * (month + 1) / 10 + date - 1) % 7;
    return 0;
}

char * week[] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};

/*******************************************************************************
 * @brief   获取星期字符串
 * @param   day 星期
 * @retval  返回 day 对应的字符串，day 越界则返回 NULL
 *******************************************************************************/
char * str_of_day(uint8_t day)
{
    if (day >= sizeof(week))
        return NULL;

    return week[day];
}
