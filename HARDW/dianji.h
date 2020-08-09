#ifndef __DIANJI_H__
#define __DIANJI_H__
#include "stm32f10x.h"
#define TIMEA 3000
#define JIAODU 140
#define AA GPIOB,GPIO_Pin_1
#define BB GPIOB,GPIO_Pin_0
#define CC GPIOA,GPIO_Pin_3
#define DD GPIOA,GPIO_Pin_0
void DIANJI_Init (void);
void ZhengZhuan(void);
void FanZhuan(void);
void dledout(u8 i);
int Door_control(int flag);

#endif
