#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"
#define GPIO_Pin_LED1 GPIO_Pin_4
#define GPIO_Pin_LED2 GPIO_Pin_6
void LED_Init (void);
void ledout(u8 i);

#endif

