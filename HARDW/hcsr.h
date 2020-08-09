//超声波硬件接口定义
#include "stm32f10x.h"
#define HCSR05_PORT     GPIOB
#define HCSR05_CLK      RCC_APB2Periph_GPIOB
#define HCSR05_TRIG     GPIO_Pin_11
#define HCSR05_ECHO     GPIO_Pin_10
//void delay_init(void);
//void delay_us(uint32_t nus);
//void delay_ms(uint16_t nms);
//extern float distance;

void hcsr05_NVIC(void);
void Hcsr05Init(void);
static void OpenTimerForHc(void);
static void CloseTimerForHc(void);
void TIM4_IRQHandler(void);
u32 GetEchoTimer(void);
float Hcsr05GetLength(void );
