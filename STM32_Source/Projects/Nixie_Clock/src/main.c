/*******************************************************************************
* @file     --> main.c
* @author   --> Lichangchun
* @version  --> V0.9
* @date     --> 7-Oct-2019
* @brief    -->
*******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "usart.h"
#include "i2c_soft.h"
#include "led.h"
#include "ds3231.h"     // 高精度时钟
#include "hv57708.h"    // 辉光管驱动
#include "key.h"        // 按键驱动
#include "buzzer.h"     // 蜂鸣器驱动
#include "display.h"
#include "multi_button.h"
#include "timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define SERIAL_DEBUG    // 串口调试开关

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t       r=138, g=43, b=226; // 彩灯颜色的 RGB 值

struct Button key0, key1, key2;   // 实例化 3 个按键

/* Private function prototypes -----------------------------------------------*/
static uint8_t key0_pin_level(void);
static uint8_t key1_pin_level(void);
static uint8_t key2_pin_level(void);

static void key0_single_clicked_handler(void *key);
static void key0_long_pressed_handler(void *key);
static void key1_single_clicked_handler(void *key);
static void key1_long_pressed_handler(void *key);
static void key2_single_clicked_handler(void *key);
static void key2_long_pressed_handler(void *key);

static void date2array(DS3231_DateTypeDef *date, uint8_t array[]);
static void array2date(DS3231_DateTypeDef *date, uint8_t array[]);
static void clock2array(DS3231_ClockTypeDef *clock, uint8_t array[]);
static void array2clock(DS3231_ClockTypeDef *clock, uint8_t array[]);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  主函数
  * @param  None
  * @retval None
*******************************************************************************/
int main(void)
{
  uint8_t       cnt = 0;
  DS3231_ClockTypeDef clock = {0};
  
  delay_init();
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置 NVIC 中断分组 2
#ifdef SERIAL_DEBUG // 如果开启串口调试则初始化 USART1
  USART1_Configuration(115200);
#endif
  I2c_Init(); // 与实时时钟(DS3231)和温湿度传感器(SHT30)通信 
  LED_Init();
  LED_Off();
  //WS2812B_Init();
  Buzzer_Init();
  Buzzer_Sound2(); // 嘀
  Display_Init();
  
  /* 按键相关初始化工作 - start */
  Keys_GPIO_Init();
  
  button_init(&key0, key0_pin_level, KEY0_PRESS_LEVEL);
  button_init(&key1, key1_pin_level, KEY1_PRESS_LEVEL);
  button_init(&key2, key2_pin_level, KEY2_PRESS_LEVEL);
  
  button_attach(&key0, SINGLE_CLICK,      key0_single_clicked_handler); // key0 短按
  button_attach(&key0, LONG_RRESS_START,  key0_long_pressed_handler); // key0 长按
  button_attach(&key1, SINGLE_CLICK,      key1_single_clicked_handler);
  button_attach(&key1, LONG_RRESS_START,  key1_long_pressed_handler);
  button_attach(&key2, SINGLE_CLICK,      key2_single_clicked_handler);
  button_attach(&key2, LONG_RRESS_START,  key2_long_pressed_handler);
  
  button_start(&key0);
  button_start(&key1);
  button_start(&key2);
  
  TIM4_Int_Init(35999, 9); // Timer4 用于 multi_button 的独立时间片轮转
  
  /* 按键相关初始化工作 - end */
  
  while(1)
  {
    delay_ms(10);
    /* 更新时钟读数 */
    DS3231_GetClock(&clock);
    /* 默认显示时间 */
    Clock_Display(&clock);
    cnt++;
    if (cnt == 50) // 每 500ms 翻转一次 LED
    {
      cnt = 0;
      LED_Flip();
    }
  }
}

/*******************************************************************************
  * @brief  返回 key0 的电平状态
*******************************************************************************/
uint8_t key0_pin_level(void)
{
  return KEY0;
}

/*******************************************************************************
  * @brief  返回 key1 的电平状态
*******************************************************************************/
uint8_t key1_pin_level(void)
{
  return KEY1;
}

/*******************************************************************************
  * @brief  返回 key2 的电平状态
*******************************************************************************/
uint8_t key2_pin_level(void)
{
  return KEY2;
}

/*******************************************************************************
  * @brief  key0 单击事件, 显示日期
*******************************************************************************/
void key0_single_clicked_handler(void *key)
{
  DS3231_DateTypeDef date;
  DS3231_GetDate(&date);
  Date_Display(&date);
  /* 延时 3s */
  delay_ms(1000);
  delay_ms(1000);
  delay_ms(1000);
}

/*******************************************************************************
  * @brief  key0 长按事件, 进入日期设置
*******************************************************************************/
void key0_long_pressed_handler(void *key)
{
  /* 进入设置后暂时关闭 Timer4, 随后将采用普通扫描模式读取按键值 */
  TIM_Cmd(TIM4, DISABLE);
  
  
  uint8_t             key_value; 
  uint16_t            null_cnt = 0; // 无按键计时, 每 10ms 递增
  uint8_t             blink = 0;    // 闪烁正在设置的位
  uint8_t             set_data[6];  // 存放正在设置的值
  uint8_t             dis_data[6];  // 显示暂存区
  uint8_t             pt = 0;       // 指向当前正在设置的位
  uint8_t             i;
  DS3231_DateTypeDef  date;
  
  DS3231_GetDate(&date);
  Date_Display(&date);
  date2array(&date, set_data);
  
  /* 等待 KEY0 键释放, 重要, 否则会立即退出设置 */
  while (KEY0 == KEY0_PRESS_LEVEL);
  
  while (1)
  {
    key_value = Keys_Scan();
    /************************************
    KEY0_PRESS: 保存并退出设置
    KEY1_PRESS: 切换设置的位
    KEY2_PRESS: 增加数值
    0         : 连续 20s 无按键退出设置
    ************************************/
    if (key_value == KEY0_PRESS)
    {
      array2date(&date, set_data);
      DS3231_SetDate(&date);
      while (KEY1 == KEY1_PRESS_LEVEL);
      break;
    }
    else if (key_value == KEY1_PRESS)
    {
      null_cnt = 0;
      /* 等待弹起, 重要 */
      while (KEY1 == KEY1_PRESS_LEVEL);
      pt++;
      if (pt == 6)
        pt = 0;
    }
    else if (key_value == KEY2_PRESS)
    {
      null_cnt = 0;
      /* 等待弹起, 重要 */
      while (KEY2 == KEY2_PRESS_LEVEL);
      set_data[pt]++;
      if (set_data[pt] == 10)
        set_data[pt] = 0;
    }
    else // 无按键按下
    {
      null_cnt++;
      if (null_cnt > 2000)
      {
        break;
      }
    }
    
    /* 闪烁正在设置的位 */
    if (null_cnt % 50 == 0)
    {
      blink = !blink;
    }
    
    /* 拷贝正在设置的段到显示暂存区 */
    for (i = 0; i < 6; i++)
      dis_data[i] = set_data[i];
      
    if (blink)
      dis_data[pt] = 11; // 大于 10 的数字将不显示
    
    HV57708_Display(dis_data); // 显示
    delay_ms(10);
  }
  /* 退出前打开 Timer4, 启用多按键时间扫描 */
  TIM_Cmd(TIM4, ENABLE);
}

/*******************************************************************************
  * @brief  key1 单击事件, 显示温度
*******************************************************************************/
void key1_single_clicked_handler(void *key)
{
}

/*******************************************************************************
  * @brief  key1 长按事件, 进入时间设置
*******************************************************************************/
void key1_long_pressed_handler(void *key)
{
  /* 进入设置后暂时关闭 Timer4, 随后将采用普通扫描模式读取按键值 */
  TIM_Cmd(TIM4, DISABLE);
  
  uint8_t             key_value; 
  uint16_t            null_cnt = 0; // 无按键计时, 每 10ms 递增
  uint8_t             blink = 0;    // 闪烁正在设置的位
  uint8_t             set_data[6];  // 存放正在设置的值
  uint8_t             dis_data[6];  // 显示暂存区
  uint8_t             pt = 0;       // 指向当前正在设置的位
  uint8_t             i;
  DS3231_ClockTypeDef clock;
  
  DS3231_GetClock(&clock);
  Clock_Display(&clock);
  clock2array(&clock, set_data);
  
  /* 等待 KEY1 键释放, 否则会不断切换设置的位 */
  while (KEY1 == KEY1_PRESS_LEVEL);
  
  while (1)
  {
    key_value = Keys_Scan();
    /************************************
    KEY0_PRESS: 保存并退出设置
    KEY1_PRESS: 切换设置的位
    KEY2_PRESS: 增加数值
    0         : 连续 20s 无按键退出设置
    ************************************/
    if (key_value == KEY0_PRESS)
    {
      array2clock(&clock, set_data);
      DS3231_SetClock(&clock);
      while (KEY1 == KEY1_PRESS_LEVEL);
      break;
    }
    else if (key_value == KEY1_PRESS)
    {
      null_cnt = 0;
      /* 等待弹起, 重要 */
      while (KEY1 == KEY1_PRESS_LEVEL);
      pt++;
      if (pt == 6)
        pt = 0;
    }
    else if (key_value == KEY2_PRESS)
    {
      null_cnt = 0;
      /* 等待弹起, 重要 */
      while (KEY2 == KEY2_PRESS_LEVEL);
      set_data[pt]++;
      if (set_data[pt] == 10)
        set_data[pt] = 0;
    }
    else // 无按键按下
    {
      null_cnt++;
      if (null_cnt > 2000)
      {
        break;
      }
    }
    
    /* 闪烁正在设置的位 */
    if (null_cnt % 50 == 0)
    {
      blink = !blink;
    }
    
    /* 拷贝正在设置的段到显示暂存区 */
    for (i = 0; i < 6; i++)
      dis_data[i] = set_data[i];
      
    if (blink)
      dis_data[pt] = 11; // 大于 10 的数字将不显示
    
    HV57708_Display(dis_data); // 显示
    delay_ms(10);
  }
  /* 退出前打开 Timer4, 启用多按键时间扫描 */
  TIM_Cmd(TIM4, ENABLE);
}

void key2_single_clicked_handler(void *key)
{
}

/*******************************************************************************
  * @brief  key2 长按, 开/关辉光管
*******************************************************************************/
void key2_long_pressed_handler(void *key)
{
  if (HV57708_TubePowerStatus())
    HV57708_TubePower(DISABLE);
  else 
    HV57708_TubePower(ENABLE);
}

/**
  * @brief  Timer4 中断. 每 5ms 调用一次 button_ticks().
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    
    button_ticks();
  }
}

/*******************************************************************************
  * @brief Date 结构转换为数组
*******************************************************************************/
void date2array(DS3231_DateTypeDef *date, uint8_t array[])
{
  array[0] = date->year   / 10;
  array[1] = date->year   % 10;
  array[2] = date->month  / 10;
  array[3] = date->month  % 10;
  array[4] = date->date   / 10;
  array[5] = date->date   % 10;
}

/*******************************************************************************
  * @brief 数组转换为 Date 结构
*******************************************************************************/
void array2date(DS3231_DateTypeDef *date, uint8_t array[])
{
  date->year   = array[0] * 10 + array[1];
  date->month = array[2] * 10 + array[3];
  date->date = array[4] * 10 + array[5];
}

/*******************************************************************************
  * @brief Clock 结构转换为数组
*******************************************************************************/
void clock2array(DS3231_ClockTypeDef *clock, uint8_t array[])
{
  array[0] = clock->hour    / 10;
  array[1] = clock->hour    % 10;
  array[2] = clock->minute  / 10;
  array[3] = clock->minute  % 10;
  array[4] = clock->second  / 10;
  array[5] = clock->second  % 10;
}

/*******************************************************************************
  * @brief 数组转换为 Clock 结构
*******************************************************************************/
void array2clock(DS3231_ClockTypeDef *clock, uint8_t array[])
{
  clock->hour   = array[0] * 10 + array[1];
  clock->minute = array[2] * 10 + array[3];
  clock->second = array[4] * 10 + array[5];
}
