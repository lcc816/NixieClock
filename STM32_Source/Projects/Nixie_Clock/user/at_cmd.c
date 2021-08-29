/**
 ******************************************************************************
 * @file    at_cmd.c
 * @author  lcc
 * @version
 * @date    29-Aug-2021
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usart.h"
#include "ds3231.h"
#include <string.h>
#include <stdio.h>
#include "nixie_misc.h"

/* AT+NTIME=21-08-29,01:53:59 */
int at_time_proc(char *arg)
{
    DS3231_TimeTypeDef time = {0};
    int ret;
    uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    ret = sscanf(arg, "%hhu-%hhu-%hhu,%hhu:%hhu:%hhu", &time.year, &time.month,\
                 &time.date, &time.hour, &time.minute, &time.second);
    if (ret != 6)
    {
        goto error;
    }
    /* check if valid */
    if (time.year > 99 || time.month > 12 || time.month < 1 || \
        time.hour > 23 || time.minute > 59 || time.second > 59)
    {
        goto error;
    }
    if (is_leap_year(time.year + 2000) && time.month == 2)
    {
        if (time.date < 1 || time.date > 29)
        {
            goto error;
        }
    }
    else
    {
        if (time.date < 1 || time.date > days[time.month - 1])
        {
            goto error;
        }
    }

    ret = date2day(time.year + 2000, time.month, time.date, &time.day);
    if (ret != 0)
    {
        goto error;
    }

    DS3231_SetTime(&time);

    printf("OK+%02hhu-%02hhu-%02hhu,%s,%02hhu:%02hhu:%02hhu\r\n", \
           time.year, time.month, time.date, str_of_day(time.day), \
           time.hour, time.minute, time.second);
    return 0;

error:
    printf("ERROR\r\n");
    return -1;
}

/* AT+NDATE=21-08-29 */
int at_date_proc(char *arg)
{
    DS3231_DateTypeDef d = {0};
    int ret;
    uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    ret = sscanf(arg, "%hhu-%hhu-%hhu", &d.year, &d.month, &d.date);
    if (ret != 3)
    {
        goto error;
    }
    /* check if valid */
    if (d.year > 99 || d.month > 12 || d.month < 1)
    {
        goto error;
    }
    if (is_leap_year(d.year + 2000) && d.month == 2)
    {
        if (d.date < 1 || d.date > 29)
        {
            goto error;
        }
    }
    else
    {
        if (d.date < 1 || d.date > days[d.month - 1])
        {
            goto error;
        }
    }

    ret = date2day(d.year + 2000, d.month, d.date, &d.day);
    if (ret != 0)
    {
        goto error;
    }

    DS3231_SetDate(&d);

    printf("OK+%02hhu-%02hhu-%02hhu,%s\r\n", \
           d.year, d.month, d.date, str_of_day(d.day));
    return 0;

error:
    printf("ERROR\r\n");
    return -1;
}

/* AT+NCLOCK=14:28:04 */
int at_clock_proc(char *arg)
{
    DS3231_ClockTypeDef c;
    int ret;

    ret = sscanf(arg, "%hhu:%hhu:%hhu", &c.hour, &c.minute, &c.second);
    if (ret != 3)
    {
        goto error;
    }

    /* check if valid */
    if (c.hour > 23 || c.minute > 59 || c.second > 59)
    {
        goto error;
    }

    DS3231_SetClock(&c);

    printf("%02hhu:%02hhu:%02hhu\r\n", c.hour, c.minute, c.second);
    return 0;

error:
    printf("ERROR\r\n");
    return -1;
}

/* AT+1#08:30:00#R */
int at_alarm_proc(char *arg)
{
    printf("%s\r\n", arg);
    return 0;
}

static struct
{
    char *name;
    int (*cmd_proc)(char *arg);
}*p_at_proc, at_support_proc[] = {
    {"NTIME",    at_time_proc },
    {"NDATE",    at_date_proc },
    {"NCLOCK",   at_clock_proc },
    {"NALARM",   at_alarm_proc },
    {NULL,      NULL}
};

int nixie_at_cmd_handler(char *cmd, char *arg)
{
    int status = 0;

    for (p_at_proc = at_support_proc; p_at_proc->name; p_at_proc++)
    {
        if (!strcmp(cmd, p_at_proc->name))
        {
            if (!p_at_proc->cmd_proc(arg))
                status = -1;
            break;
        }
    }

    if (p_at_proc->name == NULL)
    {
        printf("invalid command: %s\r\n", cmd);
    }

    return status;
}
