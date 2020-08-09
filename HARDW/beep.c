#include "stm32f10x.h"
#include "beep.h"
#include "delay.h"
void BEEP_Init (void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOX, ENABLE);	 //??PA,PD????
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_BEEP;				       //LED0-->PA.8 ????
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO????50MHz
  GPIO_Init(GPIOX, &GPIO_InitStructure);					      //?????????GPIOA.8				
  GPIO_ResetBits(GPIOX,GPIO_BEEP);	 //PA.8 ???
	delay_ms(100);
	GPIO_SetBits(GPIOX,GPIO_BEEP);	 //PA.8 ???
}

void beepout(u8 i)
{
	if(i==0) GPIO_ResetBits(GPIOX,GPIO_BEEP);	
	if(i==1) GPIO_SetBits(GPIOX,GPIO_BEEP);	
}

void BEEP_once(void)
{beepout(1);//蜂鸣器响一下
	delay_ms(50);
	beepout(0);//蜂鸣器响一下
	delay_ms(100);
	beepout(1);
}

