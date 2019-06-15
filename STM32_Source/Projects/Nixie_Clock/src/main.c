/*******************************************************************************
* @file     --> main.c
* @author   --> Lichangchun
* @version  --> 
* @date     --> 13-Jun-2019
* @brief    -->
*******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "usart.h"
#include "i2c.h"
#include "led.h"
#include "ws2812b.h"    // 全彩 LED 彩灯
#include "ds3231.h"     // 高精度时钟, I2C 驱动
#include "hv57708.h"    // 辉光管驱动
#include "key.h"        // 按键驱动

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SERIAL_DEBUG

#define DOT1_BIT    0x01 // 0001
#define DOT2_BIT    0x02 // 0010
#define DOT3_BIT    0x04 // 0100
#define DOT4_BIT    0x08 // 1000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t r=138, g=43, b=226; // 彩灯颜色的 RGB 值
FlagStatus Neon_Status;     // 记录氖泡亮灭状态
TimeTypeDef time = {0};
uint8_t second_previous;
uint8_t dis_data[6];
FlagStatus setting;         // 进入设置模式

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
  
#ifdef SERIAL_DEBUG
  printf("Time now: %u%u:%u%u:%u%u\r\n",
    dis_data[5], dis_data[4], dis_data[3], dis_data[2], dis_data[1], dis_data[0]);
#endif
  
  /* 每秒闪烁冒号 */
  if (dis_data[0] != second_previous)
    second_previous = dis_data[0];
  
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
  printf("Date now: 20%u%u-%u%u-%u%u\r\n",
    dis_data[5], dis_data[4], dis_data[3], dis_data[2], dis_data[1], dis_data[0]);
#endif
  
  /* 不显示冒号 */
  Neon_Off(DOT1_BIT | DOT2_BIT | DOT3_BIT | DOT4_BIT);
  Neon_Status = RESET;
  
  HV57708_Display(dis_data);
}

/*******************************************************************************
  * @brief  主函数
  * @param  None
  * @retval None
*******************************************************************************/
int main(void)
{
  delay_init();
#ifdef SERIAL_DEBUG
  USART1_Configuration(115200);
#endif
  I2C2_Init(0x28, 50000);
  
  LED_Init();
  LED_Off();
  WS2812B_Init();
  Neon_Init();
  HV57708_Init();
  
  uint8_t key;
  
  while(1)
  {
    /* 重设彩灯颜色 */
    WS2812B_SetPixelRGBAll(r, g, b);
    WS2812B_Show();
    /* 更新时间读数 */
    time = DS3231_GetTime();
    /* 扫描按键值 */
    key = KEY_Scan(1);
    switch (key)
    {
    }
    
    Time_Display();
    LED_Flip();
    delay_ms(500);
  }
}
