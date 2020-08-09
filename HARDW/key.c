#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
void KEY_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				  //LED0-->Pb.5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//上拉输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);					  //初始化

}

u8 KEY_Scan(u8 mode)
{     
    static u8 key_up=1;
    if(mode)key_up=1;  
   
    if(key_up&&KEy==1)
    {
        delay_ms(5);
        key_up=0;
        if(KEy==1)
        {
            return 1;
        }       
    }
    else if(KEy==0)
    {
        key_up=1;
        
    }      
		return 0; 
}

