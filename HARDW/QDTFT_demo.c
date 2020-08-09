/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "QDTFT_demo.h"
unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};
void Redraw_Mainmenu(void)
{

	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(8,0,BLUE,GRAY0,"电子模块");
	Gui_DrawFont_GBK16(8,20,RED,GRAY0,"液晶测试");

	DisplayButtonUp(0,38,80,58); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(8,40,YELLOW,GRAY0,"颜色填充");

	DisplayButtonUp(0,68,80,88); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(8,70,BLUE,GRAY0,"文字显示");

	DisplayButtonUp(0,98,80,118); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(8,100,RED,GRAY0,"图片显示");
	delay_ms(1500);
}

void Num_Test(void)
{
	u8 i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(8,20,RED,GRAY0,"Num Test");
	delay_ms(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	Gui_DrawFont_Num32((i%4)*40,32*(i/4)+5,RED,GRAY0,Num[i+1]);
	delay_ms(100);
	}
	
}
void LCD_ShowNumTES(void)
{
	u8 i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(8,20,RED,GRAY0,"SHOW_Num");
	delay_ms(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<30;i++)
	{
			LCD_ShowNum(35,0,i,3);
			LCD_ShowNum(35,20,i,3);
    	LCD_ShowNum(35,40,i,3);
			LCD_ShowNum(35,60,i,3);
	delay_ms(10);
	}
	
}


void Font_Test(void)
{
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(8,10,BLUE,GRAY0,"文字显示");

	delay_ms(1000);
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(8,0,RED,GRAY0,"液晶模块");
	Gui_DrawFont_GBK16(8,20,BLUE,GRAY0,"文字显示");
	Gui_DrawFont_GBK16(8,40,RED,GRAY0, "技术支持");
	Gui_DrawFont_GBK16(0,60,BLUE,GRAY0,"Tel:88888888888");
	Gui_DrawFont_GBK16(0,80,RED,GRAY0, "QQ:666666666");	
	delay_ms(1800);	
}

void Color_Test(void)
{
	u8 i=1;
	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(4,10,BLUE,GRAY0,"Color Test");
	delay_ms(200);

	while(i--)
	{
		Lcd_Clear(WHITE);
		Lcd_Clear(BLACK);
		Lcd_Clear(RED);
	  	Lcd_Clear(GREEN);
	  	Lcd_Clear(BLUE);
	}		
}

//取模方式 水平扫描 从左到右 低位在前
void showimage(const unsigned char *p) //显示40*40 QQ图片
{
  	int i,j,k; 
	unsigned char picH,picL;
	Lcd_Clear(WHITE); //清屏  
	
	for(k=0;k<2;k++)
	{
	   	for(j=0;j<4;j++)
		{	
			Lcd_SetRegion(40*j,40*k,40*j+39,40*k+39);		//坐标设置
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//数据低位在前
				picH=*(p+i*2+1);				
				LCD_WriteData_16Bit(picH<<8|picL);  						
			 }	
		 }
	}		
}

void QDTFT_Test_Demo(void)
{
	Lcd_Init();
		LCD_BL_SET ;
	  LCD_CS_CLR ;
 //  LCD_CS_SET ;//通过IO控制背光亮				
	Redraw_Mainmenu();//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)
	Color_Test();//简单纯色填充测试
	Num_Test();//数码管字体测试
	Font_Test();//中英文显示测试		
	//showimage(gImage_1);//图片显示示例
	delay_ms(1200);
LCD_ShowNumTES();
}
