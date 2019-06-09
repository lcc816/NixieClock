/**
  ******************************************************************************
  * @file    delay.c
  * @author  Lichangchun
  * @version 
  * @date    6-Sept-2017
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "delay.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static u8  fac_us=0;		//us	   
static u16 fac_ms=0;		//ms
//延时初始化函数
//SYSTIK的时钟固定为HCLK时钟的1/8
void delay_init(void)
{
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //选择外部时钟 HCLK/8
  fac_us=SystemCoreClock/8000000;   //为系统时钟的1/8
  fac_ms=(u16)fac_us*1000;
}

void delay_us(u32 nus)
{		
  u32 temp;	    	 
  SysTick->LOAD=nus*fac_us; 			//时间加载	 
  SysTick->VAL=0x00;		                //清空计数器
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
  do
  {
  temp=SysTick->CTRL;
  }while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
  SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
  SysTick->VAL =0X00;      			//清空计数器
}

//延时nms
//注意nms的范围
//时间SysTick->LOAD为24位寄存器,所以最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
  u32 temp;		   
  SysTick->LOAD=(u32)nms*fac_ms;		//时间加载(SysTick->LOAD为24bit)
  SysTick->VAL =0x00;				//清空计数器
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
  do
  {
  temp=SysTick->CTRL;
  }while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
  SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
  SysTick->VAL =0X00;                           //清空计数器
}
