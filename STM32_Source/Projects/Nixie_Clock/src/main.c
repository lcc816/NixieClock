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
#include "neon.h"
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
volatile FlagStatus a1_repeatable = RESET;  // 闹钟是否重复
volatile FlagStatus a2_repeatable = RESET;
volatile FlagStatus ring_flag = RESET;      // 闹铃是否响

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

static void date_setting(void);
static void clock_setting(void);
static void alarm_setting(void);
static int8_t select_setting_item(uint8_t total_items);
static void alarm_ring(void);

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
  uint8_t             cnt = 0;
  DS3231_ClockTypeDef clock = {0};
  FlagStatus          sleep_flag = RESET;
  
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
    delay_ms(20);
    /* 检查闹钟是否响 */
    if (DS3231_CheckIfAlarm(1))
    {
      DS3231_GetClock(&clock);
      Clock_Display(&clock);
      alarm_ring();
      if (!a1_repeatable) 
        DS3231_TurnOffAlarm(1); /* 如果闹钟不需重复则关闭 */
    }
    if (DS3231_CheckIfAlarm(2))
    {
      DS3231_GetClock(&clock);
      Clock_Display(&clock);
      alarm_ring();
      if (!a2_repeatable) 
        DS3231_TurnOffAlarm(2); /* 如果闹钟不需重复则关闭 */
    }
    /* 更新时钟读数 */
    DS3231_GetClock(&clock);
    /* 00:00 ~ 06:00 睡眠 */
    if (clock.hour<=6)
    {
      if (sleep_flag != SET) 
      {
        /* 睡眠期间辉光管没必要显示 */
        HV57708_TubePower(DISABLE);
        sleep_flag = SET;
      }
      else /* 但手动打开辉光管仍可正常显示 */
        Clock_Display(&clock);
    }
    else
    {
      if (sleep_flag != RESET)
      {
        HV57708_TubePower(ENABLE);
        sleep_flag = RESET;
      }
      Clock_Display(&clock); /* 默认显示时间 */
    }
    
    cnt++;
    if (cnt == 25) // 每 500ms 翻转一次 LED
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
  * @brief  key0 长按事件, 进入设置
*******************************************************************************/
void key0_long_pressed_handler(void *key)
{
  int8_t ret;
  /* 进入设置后暂时关闭 Timer4, 随后将采用普通扫描模式读取按键值 */
  TIM_Cmd(TIM4, DISABLE);
  
  ret = select_setting_item(3);
  
  switch (ret)
      {
        case 1  : date_setting();
                  break;
        case 2  : clock_setting();
                  break;
        case 3  : alarm_setting();
                  break;
        case -1 :
        default : break;
      }
  
  /* 退出前打开 Timer4, 启用多按键时间扫描 */
  TIM_Cmd(TIM4, ENABLE);
}

/*******************************************************************************
  * @brief  key1 单击事件, 显示温度
*******************************************************************************/
void key1_single_clicked_handler(void *key)
{
  float temp = DS3231_GetTemperature();
  TempOrHumi_Display(temp);
  /* 延时 3s */
  delay_ms(1000);
  delay_ms(1000);
  delay_ms(1000);
}

/*******************************************************************************
  * @brief  key1 长按事件, 开启蓝牙(todo)
*******************************************************************************/
void key1_long_pressed_handler(void *key)
{
}

/*******************************************************************************
  * @brief  key2 单击事件, 关闭闹铃或...(todo)
*******************************************************************************/
void key2_single_clicked_handler(void *key)
{
  if (ring_flag)
    ring_flag = RESET;
  else
    ;
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
  * @brief 日期设置
*******************************************************************************/
static void date_setting(void)
{
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
}

/*******************************************************************************
  * @brief 时间(Clock)设置
*******************************************************************************/
void clock_setting(void)
{
  uint8_t             key_value; 
  uint16_t            null_cnt = 0; // 无按键计时, 每 10ms 递增
  uint8_t             blink = 0;    // 闪烁正在设置的位
  uint8_t             set_data[6];  // 存放正在设置的值
  uint8_t             dis_data[6];  // 显示暂存区
  uint8_t             pt = 0;       // 指向当前正在设置的位
  uint8_t             i;
  DS3231_ClockTypeDef clock;
  
  DS3231_GetClock(&clock);
  Clock_DisplayNoBlink(&clock);
  clock2array(&clock, set_data);
  
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
}

/*******************************************************************************
  * @brief 闹钟设置
*******************************************************************************/
void alarm_setting(void)
{
  uint8_t                     key_value; 
  uint16_t                    null_cnt = 0; // 无按键计时, 每 10ms 递增
  uint8_t                     blink = 0;    // 闪烁正在设置的位
  uint8_t                     set_data[6];  // 存放正在设置的值
  uint8_t                     dis_data[6];  // 显示暂存区
  uint8_t                     pt = 0;       // 指向当前正在设置的位
  uint8_t                     i, mode;
  int8_t                      id;
  static DS3231_ClockTypeDef  clock = {0};
  DS3231_TimeTypeDef          temp;
  
  /* 选择要设置的闹钟 */
  id = select_setting_item(2);
  if (id!=1 && id!=2) return;
  
  /* 设置模式 */
  /************************************
      1: 仅一次
      2: 重复
      3: 清除闹钟
  ************************************/
  mode = select_setting_item(3);
  if (mode!=1 && mode!=2 && mode!=3) return;
  if (mode == 3)
  {
    DS3231_TurnOffAlarm(id);
    return;
  }
  
  Clock_DisplayNoBlink(&clock);
  clock2array(&clock, set_data);
  
  while (1)
  {
    key_value = Keys_Scan();
    /************************************
    KEY0_PRESS: 保存闹钟并退出
    KEY1_PRESS: 切换设置的位
    KEY2_PRESS: 增加数值
    0         : 连续 20s 无按键退出设置
    ************************************/
    if (key_value == KEY0_PRESS)
    {
      while (KEY0 == KEY0_PRESS_LEVEL);
      array2clock(&clock, set_data);
      
      temp.hour   = clock.hour;
      temp.minute = clock.minute;
      temp.second = clock.second;
      if (1 == id) /* 设置闹钟 1 */
      {
        DS3231_SetAlarm1(HourMinuteSecond, &temp);
        DS3231_TurnOnAlarm(1);
        if (1 == mode)  a1_repeatable = RESET;
        else            a1_repeatable = SET;
      }
      else         /* 设置闹钟 2 */
      {
        DS3231_SetAlarm2(HourMinute, &temp);
        DS3231_TurnOnAlarm(2);
        if (1 == mode)  a2_repeatable = RESET;
        else            a2_repeatable = SET;
      }
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

/*******************************************************************************
  * @brief  选择一个设置项
  * @param  total_items - 可选设置项总数, 应在 1 ~ 6 之间
  * @retval 返回选中的设置项, 退出设置返回 -1
*******************************************************************************/
int8_t select_setting_item(uint8_t total_items)
{
  uint8_t   key_value;
  uint8_t   sw[6] = {11, 11, 11, 11, 11, 11};
  uint8_t   pt = 1, i;
  uint16_t  null_cnt = 0; // 无按键计时, 每 10ms 递增
  uint8_t   blink = 0; // 闪烁标志
  
  if (total_items > 6) total_items = 6;
  for (i = 0; i < total_items; i++)
  {
    sw[i] = i + 1;
  }
  Neon_AllOff();
  HV57708_Display(sw);
  
  /* 等待 KEY0 键释放, 重要, 否则会立即选中 */
  while (KEY0 == KEY0_PRESS_LEVEL);
  
  while (1)
  {
    key_value = Keys_Scan();
    /************************************
    KEY0_PRESS: 返回选中的 item
    KEY1_PRESS: 右移选中的位
    KEY2_PRESS: 返回 -1 表示退出
    0         : 连续 20s 无按键返回 -1
    ************************************/
    if (key_value == KEY0_PRESS)
    {
      while (KEY0 == KEY0_PRESS_LEVEL);
      return pt;
    }
    else if (key_value == KEY1_PRESS)
    {
      while (KEY1 == KEY1_PRESS_LEVEL);
      null_cnt = 0;
      sw[pt-1] = pt;
      pt++;
      if (pt > total_items) pt = 1;
    }
    else if (key_value == KEY2_PRESS)
    {
      while (KEY2 == KEY2_PRESS_LEVEL);
      return -1; // 直接退出
    }
    else // 无按键按下
    {
      null_cnt++;
      if (null_cnt > 2000)
      {
        return -1;
      }
    }
    
    /* 闪烁正在设置的位 */
    if (null_cnt % 50 == 0)
    {
      blink = !blink;
    }
    
    if (blink)
      sw[pt-1] = 11;
    else
      sw[pt-1] = pt;
    
    HV57708_Display(sw);
    delay_ms(10);
  }
}

/*******************************************************************************
  * @brief 闹钟响铃
*******************************************************************************/
void alarm_ring(void)
{
  
  ring_flag = SET;
  while (ring_flag != RESET)
  {
    Buzzer_Sound1();
    delay_ms(1000);
  }
}
