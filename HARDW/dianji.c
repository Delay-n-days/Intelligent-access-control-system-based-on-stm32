#include "stm32f10x.h"
#include "dianji.h"
#include "delay.h"
#include "GUI.h"
#include "Lcd_Driver.h"
void DIANJI_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //??PA,PD????

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				  //LED0-->Pb.5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //??PA,PD????

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_3;		  //LED0-->Pb.5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
		GPIO_SetBits(AA);
    GPIO_SetBits(BB);
	  GPIO_SetBits(CC);
	  GPIO_SetBits(DD);
		delay_ms(100);
		GPIO_ResetBits(AA);
    GPIO_ResetBits(BB);
	  GPIO_ResetBits(CC);
	  GPIO_ResetBits(DD);
}

void ZhengZhuan(void)
{  
	u8 i;
	for(i=0;i<JIAODU;i++)
	{	
		GPIO_SetBits(AA);
    GPIO_ResetBits(DD);
	  GPIO_ResetBits(BB);
	  GPIO_ResetBits(CC);
		delay_us(TIMEA/4*3);
		GPIO_SetBits(BB);
    GPIO_ResetBits(AA);
	  GPIO_ResetBits(CC);
	  GPIO_ResetBits(DD);
		delay_us(TIMEA/4*3);
		GPIO_SetBits(CC);
    GPIO_ResetBits(AA);
	  GPIO_ResetBits(BB);
	  GPIO_ResetBits(DD);
		delay_us(TIMEA/4*3);
		GPIO_SetBits(DD);
    GPIO_ResetBits(AA);
	  GPIO_ResetBits(BB);
	  GPIO_ResetBits(CC);
		delay_us(TIMEA/4*3);
	}
}

void FanZhuan(void)
{    
		u8 i;	
   	for(i=0;i<JIAODU;i++)
	{	GPIO_SetBits(DD);
    GPIO_ResetBits(AA);
	  GPIO_ResetBits(BB);
	  GPIO_ResetBits(CC);
		delay_us(TIMEA/4*3);
		GPIO_SetBits(CC);
    GPIO_ResetBits(AA);
	  GPIO_ResetBits(BB);
	  GPIO_ResetBits(DD);
		delay_us(TIMEA/4*3);
		GPIO_SetBits(BB);
    GPIO_ResetBits(AA);
	  GPIO_ResetBits(CC);
	  GPIO_ResetBits(DD);
		delay_us(TIMEA/4*3);
		GPIO_SetBits(AA);
    GPIO_ResetBits(BB);
	  GPIO_ResetBits(CC);
	  GPIO_ResetBits(DD);
		delay_us(TIMEA/4*3);
 }
}
void dledout(u8 i)
{
	if(i==1) GPIO_SetBits(GPIOB,GPIO_Pin_13);	
	if(i==2) GPIO_SetBits(GPIOB,GPIO_Pin_5);	
	if(i==3) GPIO_SetBits(GPIOB,GPIO_Pin_6);	
	if(i==4) GPIO_SetBits(GPIOB,GPIO_Pin_7);	
	if(i==10) GPIO_ResetBits(GPIOB,GPIO_Pin_13);	
	if(i==20) GPIO_ResetBits(GPIOB,GPIO_Pin_5);	
	if(i==30) GPIO_ResetBits(GPIOB,GPIO_Pin_6);	
	if(i==40) GPIO_ResetBits(GPIOB,GPIO_Pin_7);	
}
int Door_control(int flag)
{
	if (flag==0)
  {
	Gui_DrawFont_GBK16(0,32,BLUE,WHITE,"Closing");
	FanZhuan();
	Gui_DrawFont_GBK16(0,32,BLUE,WHITE,"CLOS OK");
  }
	else if (flag==1)
	{
	Gui_DrawFont_GBK16(0,32,BLUE,WHITE,"Opening");
	ZhengZhuan();
	Gui_DrawFont_GBK16(0,32,BLUE,WHITE,"OPEN OK");
	}	return flag;
}
