#include "exti.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "delay.h"
#include "beep.h"
#include "QDTFT_demo.h"
#include "Lcd_Driver.h"
#include "GUI.h"
//按键检测中断  触发方式上升沿触发 PB4
void EXTI1_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// 按键端口初始化 
	KEY_Init();

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//GPIOE.2 中断线以及中断初始化配置   下降沿触发   
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4); 

  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//模式：中断还是事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//触发方式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);  //根据指定的参数初始化中断线参数 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//使能按键外部中断通道 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级 2，  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   //子优先级 2 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //使能外部中断通道 
  NVIC_Init(&NVIC_InitStructure); 
	
}
                      

void EXTI4_IRQHandler(void)
{int  i;
		
		delay_ms(10);
		//消抖  
		if(KEy==0)  
			//按键 KEY2 
		{  
			 i=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
		   i=!i;
       ledout(i);
		}   
	
		EXTI_ClearITPendingBit(EXTI_Line4); 
		//清除 LINE2 上的中断标志位 
} 





















