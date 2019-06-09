/**
  ******************************************************************************
  * @file    led.c
  * @author  Lichangchun
  * @version 
  * @date    6-Sept-2017
  * @brief   
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "led.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  LED所用的GPIO端口初始化
  * @param  None
  * @retval None
*******************************************************************************/
void LED_Init(void)
{
  
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);	    //使能GPIO端口时钟
  
  GPIO_InitStructure.GPIO_Pin = LED_PIN;		    //LED 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);	//根据设定参数初始化
  GPIO_SetBits(LED_GPIO_PORT,LED_PIN);			    //LED 引脚输出高
}

void LED_Off(void)
{
  GPIO_SetBits(LED_GPIO_PORT, LED_PIN);
}

void LED_On(void)
{
  GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);
}

void LED_Toggle(void)
{
  if (GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_PIN))
    GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);
  else 
    GPIO_SetBits(LED_GPIO_PORT, LED_PIN);
}
