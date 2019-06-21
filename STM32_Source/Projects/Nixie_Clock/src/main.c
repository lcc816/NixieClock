/*******************************************************************************
* @file     --> main.c
* @author   --> Lichangchun
* @version  --> 
* @date     --> 19-Jun-2019
* @brief    -->
*******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "usart.h"
#include "i2c.h"
#include "led.h"
#include "ds3231.h"     // 高精度时钟
#include "hv57708.h"    // 辉光管驱动
#include "key.h"        // 按键驱动
#include "buzzer.h"     // 蜂鸣器驱动

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SERIAL_DEBUG    // 串口调试开关

#define DOT1_BIT    0x01 // 0001
#define DOT2_BIT    0x02 // 0010
#define DOT3_BIT    0x04 // 0100
#define DOT4_BIT    0x08 // 1000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t r=138, g=43, b=226; // 彩灯颜色的 RGB 值
FlagStatus Neon_Status;     // 记录氖泡亮灭状态
TimeTypeDef time = {0};     // 记录当前时间
uint8_t second_previous;    // 前一秒读数
uint8_t dis_data[6];        // 用于显示的数据暂存区

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* @brief    --> 氖泡初始化函数
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Neon_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  Neon_Status = RESET; // 熄灭所有氖泡
  GPIO_ResetBits(GPIOB, GPIO_Pin_12); // DOT1
  GPIO_ResetBits(GPIOB, GPIO_Pin_13); // DOT2
  GPIO_ResetBits(GPIOB, GPIO_Pin_14); // DOT3
  GPIO_ResetBits(GPIOB, GPIO_Pin_15); // DOT4
}

/*******************************************************************************
* @brief    --> 点亮指定氖泡
* @param    --> dot: 取值 DOT1_BIT, DOT2_BIT, DOT3_BIT, DOT4_BIT
                      可进行或运算
* @retval   --> None
*******************************************************************************/
void Neon_On(uint8_t dot)
{
  if (dot & DOT1_BIT)
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
  
  if (dot & DOT2_BIT)
    GPIO_SetBits(GPIOB, GPIO_Pin_13);
  
  if (dot & DOT3_BIT)
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
  
  if (dot & DOT4_BIT)
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/*******************************************************************************
* @brief    --> 熄灭指定氖泡
* @param    --> dot: 取值 DOT1_BIT, DOT2_BIT, DOT3_BIT, DOT4_BIT
                      可进行或运算
* @retval   --> None
*******************************************************************************/
void Neon_Off(uint8_t dot)
{
  if (dot & DOT1_BIT)
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
  
  if (dot & DOT2_BIT)
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);
  
  if (dot & DOT3_BIT)
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
  
  if (dot & DOT4_BIT)
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

/*******************************************************************************
* @brief    --> 翻转指定氖泡
* @param    --> dot: DOT1_BIT, DOT2_BIT, DOT3_BIT, DOT4_BIT
* @retval   --> None
*******************************************************************************/
void Neon_Flip(uint8_t dot)
{
  if (dot & DOT1_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12))
      GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    else 
      GPIO_SetBits(GPIOB, GPIO_Pin_12);
  }
  if (dot & DOT2_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_13))
      GPIO_ResetBits(GPIOB, GPIO_Pin_13);
    else 
      GPIO_SetBits(GPIOB, GPIO_Pin_13);
  }
  if (dot & DOT3_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_14))
      GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    else 
      GPIO_SetBits(GPIOB, GPIO_Pin_14);
  }
  if (dot & DOT4_BIT)
  {
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_15))
      GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    else 
      GPIO_SetBits(GPIOB, GPIO_Pin_15);
  }
}

/*******************************************************************************
* @brief    --> 统一翻转所有氖泡
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Neon_FlipAll(void)
{
  Neon_Status = (FlagStatus)!Neon_Status;
  
  if (Neon_Status)
    Neon_On(DOT1_BIT | DOT2_BIT | DOT3_BIT | DOT4_BIT);
  else
    Neon_Off(DOT1_BIT | DOT2_BIT | DOT3_BIT | DOT4_BIT);
}

/*******************************************************************************
* @brief    --> 时间显示
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Time_Display(void)
{
  dis_data[0] = time.second % 10;
  dis_data[1] = time.second / 10;
  dis_data[2] = time.minute % 10;
  dis_data[3] = time.minute / 10;
  dis_data[4] = time.hour % 10;
  dis_data[5] = time.hour / 10;
  
  /* 每秒闪烁冒号 */
  if (dis_data[0] != second_previous)
  {
    second_previous = dis_data[0];
    Neon_FlipAll();
    /* 打印调试 */
  #ifdef SERIAL_DEBUG
    printf("%u%u:%u%u:%u%u\r\n",
      dis_data[5], dis_data[4], dis_data[3], dis_data[2], dis_data[1], dis_data[0]);
  #endif
  }
  
  HV57708_Display(dis_data);
}

/*******************************************************************************
* @brief    --> 日期显示
* @param    --> None
* @retval   --> None
*******************************************************************************/
void Date_Display(void)
{
  dis_data[0] = time.date % 10;
  dis_data[1] = time.date / 10;
  dis_data[2] = time.month % 10;
  dis_data[3] = time.month / 10;
  dis_data[4] = time.year % 10;
  dis_data[5] = time.year / 10;
  
#ifdef SERIAL_DEBUG
  printf("20%u%u-%u%u-%u%u\r\n",
    dis_data[5], dis_data[4], dis_data[3], dis_data[2], dis_data[1], dis_data[0]);
#endif
  
  /* 不显示冒号 */
  Neon_Off(DOT1_BIT | DOT2_BIT | DOT3_BIT | DOT4_BIT);
  Neon_Status = RESET;
  
  HV57708_Display(dis_data);
}

/*******************************************************************************
* @brief    --> Time 结构体转换成数组
* @param    --> None
* @retval   --> None
*******************************************************************************/
void time2array(uint8_t array[])
{
  array[0] = time.second % 10;
  array[1] = time.second / 10;
  array[2] = time.minute % 10;
  array[3] = time.minute / 10;
  array[4] = time.hour % 10;
  array[5] = time.hour / 10;
  
  array[6] = time.date % 10;
  array[7] = time.date / 10;
  array[8] = time.month % 10;
  array[9] = time.month / 10;
  array[10] = time.year % 10;
  array[11] = time.year / 10;
}

/*******************************************************************************
* @brief    --> 用数组设置 Time 结构体
* @param    --> None
* @retval   --> None
*******************************************************************************/
void array2time(uint8_t array[])
{
  time.second = array[0] + array[1] * 10;
  time.minute = array[2] + array[3] * 10;
  time.hour = array[4] + array[5] * 10;
  
  time.date = array[6] + array[7] * 10;
  time.month = array[8] + array[9] * 10;
  time.year = array[10] + array[11] * 10;
}

/*******************************************************************************
  * @brief  主函数
  * @param  None
  * @retval None
*******************************************************************************/
int main(void)
{
  delay_init();
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置 NVIC 中断分组 2
#ifdef SERIAL_DEBUG // 如果开启串口调试则初始化 USART1
  USART1_Configuration(115200);
#endif
  HV57708_Init();
  I2C2_Init(0x28, 50000); // 与实时时钟(DS3231)和温湿度传感器(SHT30)通信
  
  LED_Init();
  LED_Off();
  //WS2812B_Init();
  Neon_Init();
  Buzzer_Init();
  KEY_Init();
  
  uint8_t cnt = 0;
  
  while(1)
  {
    /* 重设彩灯颜色 */
    //WS2812B_SetPixelRGBAll(r, g, b);
    //WS2812B_Show();
    
    /* 更新时间读数 */
    time = DS3231_GetTime();
    
    /* 检查按键状态 */
    if (key0_long_flag) /* key0 长按, 进入设置 */
    {
      key0_long_flag = 0;
    #ifdef SERIAL_DEBUG // 如果开启串口调试则初始化 USART1
      printf("Setting...\r\n");
    #endif
      /* 进入设置模式后关闭按键的外部中断, 随后将采用普通扫描模式读取按键值 */
      KEY_EXTI_ITConfig(KEY0_EXTI_Line | KEY1_EXTI_Line | KEY2_EXTI_Line, DISABLE);
	  
      /* 等待 KEY0 键释放, 重要, 否则会立即退出设置 */
      while (KEY0 == KEY0_PRESS_LEVEL);
      
      uint8_t key_value; 
      uint16_t null_cnt = 0;// 无按键计时, 每 10ms 递增
      uint8_t blink = 0;    // 闪烁正在设置的位
      uint8_t set_data[12]; // 存放正在设置的值
      time2array(set_data);
      uint8_t pt = 0;       // 指向当前正在设置的位
      
      while (1)
      {
        key_value = KEY_Scan();
        /**************************
        KEY0_PRESS: 保存并退出设置
        KEY1_PRESS: 切换设置的位
        KEY2_PRESS: 增加数值
        0         : 连续 15s 无按键退出设置
        **************************/
        if (key_value == KEY0_PRESS)
        {
          array2time(set_data);
          DS3231_SetTime(time);
        #ifdef SERIAL_DEBUG
          printf("Save and exit setting\r\n");
        #endif
          break; // 跳出循环
        }
        else if (key_value == KEY1_PRESS)
        {
          null_cnt = 0;
          /* 等待弹起, 重要 */
          while (KEY1 == KEY1_PRESS_LEVEL);
        #ifdef SERIAL_DEBUG
          printf("Move left\r\n");
        #endif
          pt++;
          if (pt == 12)
            pt = 0;
        }
        else if (key_value == KEY2_PRESS)
        {
          null_cnt = 0;
          /* 等待弹起, 重要 */
          while (KEY2 == KEY2_PRESS_LEVEL);
        #ifdef SERIAL_DEBUG
          printf("Increase\r\n");
        #endif
          set_data[pt]++;
          if (set_data[pt] == 10)
            set_data[pt] = 0;
          
        #ifdef SERIAL_DEBUG 
          printf("%u%u-%u%u-%u%u %u%u:%u%u:%u%u\r\n",
            set_data[11], set_data[10], set_data[9], set_data[8], set_data[7], set_data[6], \
            set_data[5], set_data[4], set_data[3], set_data[2], set_data[1], set_data[0]);
        #endif
        }
        else // 无按键按下
        {
          null_cnt++;
          if (null_cnt > 1500)
          {
          #ifdef SERIAL_DEBUG 
            printf("Exit without saving\r\n");
          #endif
            break;  // 跳出循环
          }
        }
        
        /* 正在设置的位闪烁标志 */
        if (null_cnt % 50 == 0)
        {
          blink = !blink;
        }
        
        /* 拷贝正在设置的段到显示暂存区 */
        if (pt < 6) // 时间设置
        {
          for (uint8_t i = 0; i <= 5; i++)
            dis_data[i] = set_data[i];
          
          if (blink)
            dis_data[pt] = 10; // 大于 9 的数字将不显示, 从而制造闪烁
        }
        else        // 日期设置
        {
          for (uint8_t i = 0; i <= 5; i++)
            dis_data[i] = set_data[6 + i];
          
          if (blink)
            dis_data[pt-6] = 10; // 大于 9 的数字将不显示
        }

        HV57708_Display(dis_data); // 显示
        delay_ms(10);
      }
      /* 退出设置前打开外部中断 */
      KEY_EXTI_ITConfig(KEY0_EXTI_Line | KEY1_EXTI_Line | KEY2_EXTI_Line, ENABLE);
    }
    
    if (key0_short_flag) /* key0 短按, 显示日期 */
    {
      key0_short_flag = 0;
      Date_Display();
      /* 延时 3s */
      delay_ms(1000);
      delay_ms(1000);
      delay_ms(1000);
    }
    
    if (key1_short_flag) /* key1 短按, 显示温度 */
    {
      key1_short_flag = 0;
    #ifdef SERIAL_DEBUG
      printf("temperature:\r\n");
    #endif
       /* 延时 3s */
      delay_ms(1000);
      delay_ms(1000);
      delay_ms(1000);
    }
      
    if (key2_short_flag) /* key2 短按, 显示湿度 */
    {
      key2_short_flag = 0;
    #ifdef SERIAL_DEBUG
      printf("humidity:\r\n");
    #endif
       /* 延时 3s */
      delay_ms(1000);
      delay_ms(1000);
      delay_ms(1000);
    }
    
    /* 默认显示时间 */
    Time_Display();
    cnt++;
    if (cnt == 50) // 每 500ms 翻转一次 LED
    {
      cnt = 0;
      LED_Flip();
    }
    delay_ms(10);
  }
}
