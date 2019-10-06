/*******************************************************************************
* @file     --> key.c
* @author   --> Lichangchun
* @version  --> 1.1
* @date     --> 22-Jun-2019
* @brief    --> 按键驱动, 通过外部中断检测 KEY0, KEY1 和 KEY2 的状态, 设置相应键
*               的长按、短按标志
*******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "delay.h"
#include "timer.h"

/* Global variables ----------------------------------------------------------*/
/* 按键触发标志量, 全局变量 */
volatile uint8_t key0_short_flag = 0;
volatile uint8_t key0_long_flag = 0;
volatile uint8_t key1_short_flag = 0;
volatile uint8_t key1_long_flag = 0;
volatile uint8_t key2_short_flag = 0;
volatile uint8_t key2_long_flag = 0;

/* Private variables ---------------------------------------------------------*/
volatile uint8_t key0_press = 0;
volatile uint8_t key1_press = 0;
volatile uint8_t key2_press = 0;

/* Private functions ---------------------------------------------------------*/
void EXTIX_Init(void);

/*******************************************************************************
* @brief    --> 按键 GPIO 配置
* @param    --> None
* @retval   --> None
*******************************************************************************/
void KEY_Init(void)
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
  
  /* 启用 Timer4, 每 1ms 中断一次, 即 1000Hz */
  TIM4_Int_Init(35999, 1); // f = 72000000 / (36000 * 2)
  /* 配置并开启引脚的外部中断 */
  EXTIX_Init();
}

/*******************************************************************************
                              配置外部中断
*******************************************************************************/
void EXTIX_Init(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* KEY0 */
  GPIO_EXTILineConfig(KEY0_GPIO_PortSource, KEY0_GPIO_PinSource);// 设置 IO 口与中断线的映射关系
  EXTI_InitStructure.EXTI_Line = KEY0_EXTI_Line;
  if (KEY0_PRESS_LEVEL != 0)  // 上升沿触发
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  else                        // 下降沿触发
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStructure);
  
  /* KEY1 */
  GPIO_EXTILineConfig(KEY1_GPIO_PortSource, KEY1_GPIO_PinSource);
  EXTI_InitStructure.EXTI_Line = KEY1_EXTI_Line;
  if (KEY1_PRESS_LEVEL != 0)
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  else
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStructure);
  
  /* KEY2 */
  GPIO_EXTILineConfig(KEY2_GPIO_PortSource, KEY2_GPIO_PinSource);
  EXTI_InitStructure.EXTI_Line = KEY2_EXTI_Line;
  if (KEY2_PRESS_LEVEL != 0) 
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  else
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = KEY0_EXIT_IRQn;	//使能按键所在的外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;	//子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXIT_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;	//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXIT_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;	//子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* @brief    --> 开启或关闭按键引脚的外部中断
* @param    --> EXTI_Line: 外部中断线
*               NewState: 使能 or 失能
* @retval   --> None
*******************************************************************************/
void KEY_EXTI_ITConfig(uint32_t EXTI_Line, FunctionalState NewState)
{
  uint32_t addr = (uint32_t)EXTI_BASE; // 中断屏蔽寄存器地址
  
  if (NewState != DISABLE) // 使能
  {
    *(__IO uint32_t *) addr |= EXTI_Line;
  }
  else
  {
    *(__IO uint32_t *) addr &= ~EXTI_Line;
  }
}

/*******************************************************************************
* @brief    --> 按键扫描函数
* @param    --> None
* @retval   --> KEY0_PRESS: KEY0 被按下
*               KEY1_PRESS: KEY1 被按下
*               KEY2_PRESS: KEY2 被按下
*******************************************************************************/
uint8_t KEY_Scan(void)
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

/*******************************************************************************
                        KEY0/1/2 外部中断服务函数
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
  if (EXTI_GetITStatus(KEY0_EXTI_Line) != RESET)
  {
    EXTI_ClearITPendingBit(KEY0_EXTI_Line); // 清除 KEY0 的外部中断标志位
    if (KEY0 == KEY0_PRESS_LEVEL)
    {
      key0_press = 1; // 生成按键按下标志
    }
  }
}

void EXTI9_5_IRQHandler(void)
{
  if (EXTI_GetITStatus(KEY1_EXTI_Line) != RESET)
  {
    EXTI_ClearITPendingBit(KEY1_EXTI_Line);
    if(KEY1 == KEY1_PRESS_LEVEL)
    {
      key1_press = 1;
    }
  }
  if (EXTI_GetITStatus(KEY2_EXTI_Line) != RESET)
  {
    EXTI_ClearITPendingBit(KEY2_EXTI_Line);
    if (KEY2 == KEY2_PRESS_LEVEL)
    {
      key2_press = 1;
    }
  }
}

/*******************************************************************************
                      定时器 4 中断服务函数, 每 1 ms 中断一次
*******************************************************************************/
void TIM4_IRQHandler(void)
{
  static uint16_t key0_hold_ms = 0;
  static uint16_t key1_hold_ms = 0;
  static uint16_t key2_hold_ms = 0;
  
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) // 检查 TIM4 更新中断发生与否
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
   
    /********* key0 发生按下事件 *********/
    if (key0_press != 0) 
    {
      if (KEY0 == KEY0_PRESS_LEVEL)/* key0 仍是按下的 */
      {
        if (key0_hold_ms < 2000) // 按下小于 2000ms 
        {
          key0_hold_ms++;
        }
        else // 一旦按下超过 2000ms 就判定为长按
        {
          key0_press = 0;
          key0_hold_ms = 0;
          key0_short_flag = 0; // 清除短按标志
          key0_long_flag = 1;  // 置位长按标志
        }
      }
      else  /* key0 已释放 */
      {
        key0_press = 0;        
        if (key0_hold_ms > 30) // 大于 30ms, 表示单击
        {
          key0_hold_ms = 0;
          key0_short_flag = 1; // 置位短按标志
          key0_long_flag = 0;  // 清除长按标志
        }
        else                   // 小于 30ms, 消抖
        {
          key0_hold_ms = 0;
          key0_short_flag = 0; // 清除短按标志
          key0_long_flag = 0;  // 清除长按标志
        }
      }
    }
    
    /********* key1 发生按下事件 *********/
    if (key1_press != 0)
    {
      if (KEY1 == KEY1_PRESS_LEVEL)/* key1 仍是按下的 */
      {
        if (key1_hold_ms < 2000) // 按下小于 2000ms 
        {
          key1_hold_ms++;
        }
        else // 一旦按下超过 2000ms 就判定为长按
        {
          key1_press = 0;
          key1_hold_ms = 0;
          key1_short_flag = 0; // 清除短按标志
          key1_long_flag = 1;  // 置位长按标志
        }
      }
      else /* key1 已释放 */
      {
        key1_press = 0;        
        if (key1_hold_ms > 30) // 大于 30ms, 表示单击
        {
          key1_hold_ms = 0;
          key1_short_flag = 1; // 置位短按标志
          key1_long_flag = 0;  // 清除长按标志
        }
        else                   // 小于 30ms, 消抖
        {
          key1_hold_ms = 0;
          key1_short_flag = 0; // 清除短按标志
          key1_long_flag = 0;  // 清除长按标志
        }
      }
    }
    
    /********* key2 发生按下事件 *********/
    if (key2_press != 0)
    {
      if (KEY2 == KEY2_PRESS_LEVEL)/* key2 仍是按下的 */
      {
        if (key2_hold_ms < 2000) // 按下小于 2000ms 
        {
          key2_hold_ms++;
        }
        else // 一旦按下超过 2000ms 就判定为长按
        {
          key2_press = 0;
          key2_hold_ms = 0;
          key2_short_flag = 0; // 清除短按标志
          key2_long_flag = 1;  // 置位长按标志
        }
      }
      else /* key1 已释放 */
      {
        key2_press = 0;        
        if (key2_hold_ms > 30) // 大于 30ms, 表示单击
        {
          key2_hold_ms = 0;
          key2_short_flag = 1; // 置位短按标志
          key2_long_flag = 0;  // 清除长按标志
        }
        else                   // 小于 30ms, 消抖
        {
          key2_hold_ms = 0;
          key2_short_flag = 0; // 清除短按标志
          key2_long_flag = 0;  // 清除长按标志
        }
      }
    } 
  }
}
