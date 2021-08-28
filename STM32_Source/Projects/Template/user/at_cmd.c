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
#include <string.h>

/* AT+NTIME=21-08-29,01:53:59 */
int at_time_proc(char *arg)
{
    printf("%s\r\n", arg);
    return 0;
}

int at_date_proc(char *arg)
{
    printf("%s\r\n", arg);
    return 0;
}

int at_clock_proc(char *arg)
{
    printf("%s\r\n", arg);
    return 0;
}

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
