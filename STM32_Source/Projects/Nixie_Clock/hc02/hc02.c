/*******************************************************************************
 * @file    hc02.c
 * @author  LCC
 * @version V1.0
 * @date    21-Aug-2021
 * @brief   提供 HC-02 配置接口
 *******************************************************************************/

#include "hc02.h"

/*******************************************************************************
 * @brief   HC-02 GPIO 初始化
 * @param   None
 * @retval  None
 *******************************************************************************/
void HC02_Init(void)
{
    RCC_APB2PeriphClockCmd(HC02_KEY_PCC | HC02_STA_PCC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = HC02_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /* 推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HC02_KEY_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = HC02_STA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /* 浮空输入 */
    GPIO_Init(HC02_STA_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
 * @brief   HC-02 设置 AT 指令模式
 * @param   NewState 使能或关闭 AT 指令模式
 * @retval  None
 *******************************************************************************/
void HC02_ATCmd(FunctionalState NewState)
{
    if (DISABLE != NewState)
        GPIO_SetBits(HC02_KEY_PORT, HC02_KEY_PIN);
    else
        GPIO_ResetBits(HC02_KEY_PORT, HC02_KEY_PIN);
}
