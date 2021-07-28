/*******************************************************************************
* @file     --> key.c
* @author   --> Lichangchun
* @version  --> 1.2
* @date     --> 6-Oct-2019
* @brief    --> 按键驱动, 扫描检测 KEY0, KEY1 和 KEY2 的状态
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "delay.h"
#include "timer.h"

/* Global variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* @brief    --> 按键 GPIO 配置
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Keys_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* 使能端口时钟 */
    RCC_APB2PeriphClockCmd(KEY0_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(KEY2_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = KEY0_PIN;         // KEY0
    /* 上拉 or 下拉, 与按键接法有关, 此处不用修改 */
    if (KEY0_PRESS_LEVEL != 0)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
    else
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;         // KEY1
    if (KEY1_PRESS_LEVEL != 0)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
    else
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = KEY2_PIN;         // KEY2
    if (KEY2_PRESS_LEVEL != 0)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
    else
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* @brief    --> 按键扫描函数
* @param    --> None
* @retval   --> KEY0_PRESS: KEY0 被按下
*               KEY1_PRESS: KEY1 被按下
*               KEY2_PRESS: KEY2 被按下
*******************************************************************************/
uint8_t Keys_Scan(void)
{
    if (KEY0==KEY0_PRESS_LEVEL | KEY1==KEY1_PRESS_LEVEL | KEY2==KEY2_PRESS_LEVEL)
    {
        delay_ms(10);   // 去抖动
        if (KEY0 == KEY0_PRESS_LEVEL)
            return KEY0_PRESS;
        else if (KEY1 == KEY1_PRESS_LEVEL)
            return KEY1_PRESS;
        else if (KEY2 == KEY2_PRESS_LEVEL)
            return KEY2_PRESS;
    }
    return 0;
}
