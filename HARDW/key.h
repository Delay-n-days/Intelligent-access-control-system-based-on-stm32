#ifndef __KEY_H__
#define __KEY_H__
#include "stm32f10x.h"
#define KEy GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)
#define KEY0_PRES 1//°´¼ü°´ÏÂ

void KEY_Init(void);
u8 KEY_Scan (u8);
#endif

