#include "timer.h"
#include "led.h"
#include "Lcd_Driver.h"
#include "p.h"
//通用定时器 3 中断初始化
//这里时钟选择为 APB1 的 2 倍，而 APB1 为 36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器 3!
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef
		TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		//①时钟 TIM3 使能
																//定时器 TIM3 初始化
	TIM_TimeBaseStructure.TIM_Period = arr;						//设置自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//设置时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//②初始化 TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //③允许更新中断
	//中断优先级 NVIC 设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  //TIM3 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级 0 级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //从优先级 3 级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ 通道被使能
	NVIC_Init(&NVIC_InitStructure);							  //④初始化 NVIC 寄存器
	TIM_Cmd(TIM3, ENABLE);									  //⑤使能 TIM3
}
//定时器 3 中断服务程序⑥
void TIM3_IRQHandler(void) //TIM3 中断
{
	int i;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查 TIM3 更新中断发生与否
	{
		i = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
		i = !i;
		ledout(i + 10);
		//	showimageALL(gImage_2);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除 TIM3 更新中断标志
	}
}

void TIM1_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //①使能定时器 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //①使能 GPIO 和 AFIO 复用功能时钟

	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE); //②（Partial）部分重映射 TIM3_CH2

	//设置该引脚为复用输出功能,输出 TIM1 CH4 的 PWM 脉冲波形 GPIOA.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //①初始化 GPIO //初始化 TIM1

	TIM_TimeBaseStructure.TIM_Period = arr;						//设置在自动重装载周期值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数模
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);				//③初始化 TIMx    //初始化 TIM3 Channel2 PWM 模式

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //选择 PWM 模式 2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //输出极性高
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);					  //④初始化外设 TIM3 OC4

	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_Cmd(TIM1, ENABLE);							  //⑤使能 TIM1 }
}

void TIM3_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //①使能定时器 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //①使能 GPIO 和 AFIO 复用功能时钟

	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //②（Partial）部分重映射 TIM3_CH2

	//设置该引脚为复用输出功能,输出 TIM1 CH4 的 PWM 脉冲波形 GPIOA.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		//TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //①初始化 GPIO //初始化 TIM1

	TIM_TimeBaseStructure.TIM_Period = arr;						//设置在自动重装载周期值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数模
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//③初始化 TIMx    //初始化 TIM3 Channel2 PWM 模式

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //选择 PWM 模式 2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //输出极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);					  //④初始化外设 TIM3 OC4

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_Cmd(TIM3, ENABLE);							  //⑤使能 TIM1 }
}

void TIM4_PWM_Init(u16 arr, u16 psc)
{
	//**结构体声明**//
	GPIO_InitTypeDef GPIO_InitStructure;		   //声明GPIO
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; //声明定时器
	TIM_OCInitTypeDef TIM_OCInitStructure;		   //声明PWM通道

	//**时钟使能**//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //使能定时器TIM4时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟

	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		  //LED0-->PB.6 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  //根据设定参数初始化GPIOB.6
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		  //LED0-->PB.7 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  //根据设定参数初始化GPIOB.7
	GPIO_SetBits(GPIOB, GPIO_Pin_7);

	//初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr;						//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM3 Channel1 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);					  //根据T指定的参数初始化外设TIM3 OC1

	//初始化TIM3 Channel2 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);					  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); //使能TIM4_CH1预装载寄存器
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); //使能TIM4_CH2预装载寄存器

	TIM_Cmd(TIM4, ENABLE); //使能TIM3
}
