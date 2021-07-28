/*******************************************************************************
* @file     --> buzzer.c
* @author   --> Lichangchun
* @version  --> v1.0
* @date     --> 27-Jun-2019
* @brief    --> 蜂鸣器接在 PC8, 为 TIM3_CH3 重映射后的引脚
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "buzzer.h"
#include "timer.h"
#include "delay.h"

typedef enum
{
    PIN_MODE_GPIO,
    PIN_MODE_PWM,
} PinMode_Type;

// 蜂鸣器不工作时将引脚切换为推挽输出, 以免发出兹兹声
static void pinModeSwitch(PinMode_Type mode);

/*******************************************************************************
* @brief    --> 蜂鸣器的定时器通道初始化
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Buzzer_Init(void)
{
    TIM3_PWM_Init(28799, 0);	 //不分频. PWM频率=72000000/28800=2500Hz
    TIM_SetCompare3(TIM3, 0);
    pinModeSwitch(PIN_MODE_GPIO);
}

/*******************************************************************************
* @brief    --> 嘀嘀嘀
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Buzzer_Sound1(void)
{
    pinModeSwitch(PIN_MODE_PWM);
    TIM_SetCompare3(TIM3, 5000);
    delay_ms(150);
    TIM_SetCompare3(TIM3, 0);
    delay_ms(100);
    TIM_SetCompare3(TIM3, 5000);
    delay_ms(150);
    TIM_SetCompare3(TIM3, 0);
    delay_ms(100);
    TIM_SetCompare3(TIM3, 5000);
    delay_ms(150);
    TIM_SetCompare3(TIM3, 0);
    pinModeSwitch(PIN_MODE_GPIO);
}

/*******************************************************************************
* @brief    --> 嘀
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Buzzer_Sound2(void)
{
    pinModeSwitch(PIN_MODE_PWM);
    TIM_SetCompare3(TIM3, 5000);
    delay_ms(150);
    TIM_SetCompare3(TIM3, 0);
    pinModeSwitch(PIN_MODE_GPIO);
}

void pinModeSwitch(PinMode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // TIM3_CH3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if (mode == PIN_MODE_PWM)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    }
    else
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    }
    GPIO_Init(GPIOC, &GPIO_InitStructure); // 重新初始化 GPIO
}
