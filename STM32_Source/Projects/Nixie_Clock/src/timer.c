  /*******************************************************************************
* @file     --> timer.c
* @author   --> Lichangchun
* @version  --> v1.0
* @date     --> 16-Jun-2019
* @brief    --> 定时器配置. Tout = ((arr+1)*(psc+1))/Fclk
*               启用 Timer4 通用定时器
*               启用 TIM3_CH1 作 PWM 输出, 并将 TIM3_CH1 重映射到 PC6
*******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "timer.h"

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* @brief    --> 定时器中断配置
* @param    --> arr: 自动重装值
*               psc: 时钟预分频数
* @retval   --> None
*******************************************************************************/
void TIM4_Int_Init(uint16_t arr, uint16_t psc)
{
  /* 时基初始化 */
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;    // 时基配置
  NVIC_InitTypeDef NVIC_InitStructure;              // 中断优先级配置
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 使能定时器时钟
  TIM_TimeBaseStructure.TIM_Prescaler = psc;
  TIM_TimeBaseStructure.TIM_Period = arr;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 设置时钟分频因子
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  // 使能更新中断
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; // TIMx 中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级3级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_Cmd(TIM4, ENABLE);                      //使能 TIMx 外设
}

/*******************************************************************************
* @brief    --> PWM 通道配置
* @param    --> arr: 自动重装值
*               psc: 时钟预分频数
* @retval   --> None
*******************************************************************************/
void TIM3_PWM_Init(uint16_t arr, uint16_t psc)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;     // TIM3 CH1
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	// 使能 TIM3 时钟
  /* 要将 TIM3_CH1 重映射到 PC6, 要同时开启 GPIOC 和 AFIO 的时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);  // 重映射 TIM3_CH1->PC6
  
  /* 设置该引脚为复用输出功能, 输出 TIM3_CH1 的 PWM 脉冲波形	GPIOC.6 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;       // TIM3_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);          // 初始化 GPIO
  
  /* 初始化TIM3 */
  TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
  /* 初始化 TIM3_CH1 的 PWM 模式 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM PWM模式2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性: 高电平有效
  TIM_OC1Init(TIM3, &TIM_OCInitStructure); //根据TIM_OCInitStructure指定的参数初始化外设TIM3的输出比较1
  
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); //使能预装载寄存器
  TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}
