/**
  ******************************************************************************
  * @file    template.c
  * @author  Lichangchun
  * @version 
  * @date    13-April-2017
  * @brief   
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "usart.h"
#include "i2c.h"
#include "led.h"
#include "ws2812b.h"    // 全彩 LED 彩灯
#include "ds3231.h"     // 高精度时钟, I2C 驱动

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  主函数
  * @param  None
  * @retval None
*******************************************************************************/
int main(void)
{
  delay_init();
  USART1_Configuration(115200);
  I2C1_Init(0x28, 50000);
  
  LED_Init();
  LED_Off();
  //RGB_LED_Init();
  //RGB_LED_Clear();
  
  TimeType time = { 19, 6, 9, 7, 17, 8, 0};
  DS3231_SetTime(time);
  
  while(1)
  {
    //RGB_LED_Rainbow(10);
    
    time = DS3231_GetTime();
    printf("Time now: 20%02u-%02u-%02u %02u:%02u:%02u\r\n",
       time.year, time.month, time.date, time.hour, time.minute, time.second);
    LED_Toggle();
    delay_ms(1000);
  }
}
