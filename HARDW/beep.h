#ifndef __BEEP_H__
#define __BEEP_H__
#include "stm32f10x.h"
#define GPIOX GPIOB
#define GPIO_BEEP GPIO_Pin_5
#define RCC_APB2Periph_GPIOX RCC_APB2Periph_GPIOB
void BEEP_Init (void);
void beepout(u8 i);
void BEEP_once(void);
#endif
