#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LED1 | GPIO_Pin_LED2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_LED1 | GPIO_Pin_LED2);
	delay_ms(100);
	GPIO_SetBits(GPIOB, GPIO_Pin_LED1 | GPIO_Pin_LED2);
}

void ledout(u8 i)
{
	if (i == 0)
		GPIO_ResetBits(GPIOB, GPIO_Pin_LED1);
	if (i == 1)
		GPIO_SetBits(GPIOB, GPIO_Pin_LED1);
	if (i == 10)
		GPIO_ResetBits(GPIOB, GPIO_Pin_LED2);
	if (i == 11)
		GPIO_SetBits(GPIOB, GPIO_Pin_LED2);
}
