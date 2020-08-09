#ifndef __RENTI_H__
#define __RENTI_H__
#include "stm32f10x.h"
#define RE0 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)
#define RE0_PRES 1 //°´¼ü°´ÏÂ

void RE_Init(void);
u8 RE_Scan(u8);
#endif
