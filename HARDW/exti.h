#ifndef __EXTI_H__
#define __EXTI_H__
#include "stm32f10x.h"
#define KEY1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define KEY0_PRES 1//°´¼ü°´ÏÂ

void EXTI1_Init(void);
#endif
