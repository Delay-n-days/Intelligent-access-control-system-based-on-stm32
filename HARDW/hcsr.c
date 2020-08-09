#include "hcsr.h"
#include "misc.h"
#include "stm32f10x.h"
#include "delay.h"
//#include "bsp_usart.h"
//?????

uint16_t  msHcCount = 0;




//void delay_init()
//{
//    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //选择外部时钟元
//    fac_us=SystemCoreClock/8000000; // 72/8 延时1微秒 9个时钟周期
//    fac_ms=(u16)fac_us*1000;   // 延时1个毫秒9000个Cystic时钟周期
//		printf("delay\n");
//}


///**
// * nus : ??????
// **/
//void delay_us(u32 nus)
//{
//    u32 temp;
//    //nus*fac_us???????SysTick->LOAD(24?)-1
//    SysTick->LOAD=nus*fac_us;    // ?????:n(us)*??1us?????SysTick????
//    SysTick->VAL=0x00;                       // VAL????0
//    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; // ??SysTick???
//    do
//    {
//        temp=SysTick->CTRL;
//    }while((temp&0x01)&&!(temp&(1<<16)));    // ????????(?16)
//    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; // ????
//    SysTick->VAL =0X00;                      // ??VAL
//}

///**
// * nms : ??????
// **/
//void delay_ms(u16 nms)
//{
//    u32 temp;
//    SysTick->LOAD=(u32)nms*fac_ms;
//    SysTick->VAL =0x00;
//    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
//    do
//    {
//        temp=SysTick->CTRL;
//    }while((temp&0x01)&&!(temp&(1<<16)));
//    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
//    SysTick->VAL =0X00;
//}



//定时器4设置
void hcsr05_NVIC()
{
		NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;             
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       
		NVIC_Init(&NVIC_InitStructure);
}


//IO口及定时器初始化
void Hcsr05Init()
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;   
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(HCSR05_CLK, ENABLE);
   
    GPIO_InitStructure.GPIO_Pin =HCSR05_TRIG;      
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(HCSR05_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR05_PORT,HCSR05_TRIG);
     
    GPIO_InitStructure.GPIO_Pin =   HCSR05_ECHO;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HCSR05_PORT, &GPIO_InitStructure);  
    GPIO_ResetBits(HCSR05_PORT,HCSR05_ECHO);    
     
          
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   
     
    TIM_DeInit(TIM4);  //复位
    TIM_TimeBaseStructure.TIM_Period = (1000-1); 
    TIM_TimeBaseStructure.TIM_Prescaler =(72-1);   //1MS进入一次中断
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);          
        
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);  
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);    
    hcsr05_NVIC();
    TIM_Cmd(TIM4,DISABLE);    
		//printf("hcsr05\n");
}




//打开定时器
static void OpenTimerForHc()  
{
   TIM_SetCounter(TIM4,0);
   msHcCount = 0;
   TIM_Cmd(TIM4, ENABLE); 
}


//关闭定时器
static void CloseTimerForHc()    
{
   TIM_Cmd(TIM4, DISABLE); 
}


//定时器中断函数
void TIM4_IRQHandler(void)  
{
   if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  
   {
       TIM_ClearITPendingBit(TIM4, TIM_IT_Update  ); 
       msHcCount++;
   }
}
 


//获取定时器计数值
u32 GetEchoTimer(void)
{
   u32 t = 0;
   t = msHcCount*1000;
   t += TIM_GetCounter(TIM4);
//   TIM4->CNT = 0;  
	TIM_SetCounter(TIM4,0);
   delay_ms(50);
   return t;
}
 
//通过定时器计数的值计算距离
//float distance;
float Hcsr05GetLength(void )
{
   u32 t = 0;
   int i = 0;
   float lengthTemp = 0;
   float sum = 0;
//   while(i!=5)
//   {

			GPIO_SetBits(HCSR05_PORT,HCSR05_TRIG); //拉高TRIG脚 发送超过10us的高电平
		  delay_us(20);
			GPIO_ResetBits(HCSR05_PORT,HCSR05_TRIG); //拉低TRIG
//      OpenTimerForHc();         //打开定时器
      i = i + 1;
      while(GPIO_ReadInputDataBit(HCSR05_PORT,HCSR05_ECHO) == 0);  //等待ECHO脚高电平 跳出while循环
			OpenTimerForHc();         //打开定时器
			while(GPIO_ReadInputDataBit(HCSR05_PORT,HCSR05_ECHO) == 1){} //等待ECHO脚高电平 跳出while循环
      CloseTimerForHc();         //关闭定时器
      t = GetEchoTimer();        //获取定时器计数的值
      lengthTemp = ((float)t/58.0);//cm
//		 lengthTemp=((float)t*0.34)/2;//cm
//			distance=0.17*t;			//距离计算
      sum = lengthTemp + sum ;
        
//    }
//    lengthTemp = sum/5.0;
    return lengthTemp;
}

