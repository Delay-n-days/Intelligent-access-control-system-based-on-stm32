#ifndef __TIMER_H__
#define __TIMER_H__
#include "stm32f10x.h"

void TIM3_Int_Init(u16 arr, u16 psc);

void TIM1_PWM_Init(u16 arr, u16 psc);
void TIM3_PWM_Init(u16 arr, u16 psc);
void TIM4_PWM_Init(u16 arr, u16 psc);
#endif
