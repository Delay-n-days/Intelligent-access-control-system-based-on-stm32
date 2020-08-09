#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "LCD_Config.h"
#include "delay.h"
#include "usart.h"
//������������STM32F103C8
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PA5��SCL��
//              SDA   ��PA7��SDA��
//              RES   ��PB0
//              DC    ��PB1
//              CS    ��PA4//����ֱ�ӽӵ�
//              BLK    ��P10 BLK�������ղ��ӣ�����Ҫ�رձ����ʱ��BLK�õ͵�ƽ
//Һ��IO��ʼ������
void LCD_GPIO_Init1(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_12 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	LCD_GPIO_Init1();
}

//��SPI���ߴ���һ��8λ����
void SPI_WriteData(u8 Data)
{
	unsigned char i = 0;
	for (i = 8; i > 0; i--)
	{
		if (Data & 0x80)
			LCD_SDA_SET; //�������
		else
			LCD_SDA_CLR;

		LCD_SCL_CLR;
		LCD_SCL_SET;
		Data <<= 1;
	}
}

//��Һ����дһ��8λָ��
void Lcd_WriteIndex(u8 Index)
{
	//SPI д����ʱ��ʼ
	LCD_RS_CLR;
	SPI_WriteData(Index);
}

//��Һ����дһ��8λ����
void Lcd_WriteData(u8 Data)
{
	LCD_RS_SET;
	SPI_WriteData(Data);
}
//��Һ����дһ��16λ����
void LCD_WriteData_16Bit(u16 Data)
{

	LCD_RS_SET;
	SPI_WriteData(Data >> 8); //д���8λ����
	SPI_WriteData(Data);	  //д���8λ����
}

void Lcd_WriteReg(u8 Index, u8 Data)
{
	Lcd_WriteIndex(Index);
	Lcd_WriteData(Data);
}

void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_ms(100);
	LCD_RST_SET;
	delay_ms(50);
}

//LCD Init For 1.44Inch LCD Panel with ST7735R.
void Lcd_Init(void)
{
	LCD_GPIO_Init();
	Lcd_Reset(); //Reset before LCD Init.

	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	Lcd_WriteIndex(0x11); //Sleep exit
	delay_ms(120);
	Lcd_WriteIndex(0x21);
	Lcd_WriteIndex(0x21);

	Lcd_WriteIndex(0xB1);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3A);
	Lcd_WriteData(0x3A);

	Lcd_WriteIndex(0xB2);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3A);
	Lcd_WriteData(0x3A);

	Lcd_WriteIndex(0xB3);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3A);
	Lcd_WriteData(0x3A);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3A);
	Lcd_WriteData(0x3A);

	Lcd_WriteIndex(0xB4);
	Lcd_WriteData(0x03);

	Lcd_WriteIndex(0xC0);
	Lcd_WriteData(0x62);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x04);

	Lcd_WriteIndex(0xC1);
	Lcd_WriteData(0xC0);

	Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x0D);
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0xC3);
	Lcd_WriteData(0x8D);
	Lcd_WriteData(0x6A);

	Lcd_WriteIndex(0xC4);
	Lcd_WriteData(0x8D);
	Lcd_WriteData(0xEE);

	Lcd_WriteIndex(0xC5); /*VCOM*/
	Lcd_WriteData(0x0E);

	Lcd_WriteIndex(0xE0);
	Lcd_WriteData(0x10);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x0A);
	Lcd_WriteData(0x12);
	Lcd_WriteData(0x27);
	Lcd_WriteData(0x37);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x0D);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0xE1);
	Lcd_WriteData(0x10);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x0F);
	Lcd_WriteData(0x06);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x0A);
	Lcd_WriteData(0x13);
	Lcd_WriteData(0x26);
	Lcd_WriteData(0x36);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x0D);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0x3A);
	Lcd_WriteData(0x05);

	Lcd_WriteIndex(0x36);
	Lcd_WriteData(0xA8); //

	Lcd_WriteIndex(0x29);
}

/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void Lcd_SetRegion(u16 x_start, u16 y_start, u16 x_end, u16 y_end)
{
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start + 1);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end + 1);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start + 0x1A);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end + 0x1A);
	Lcd_WriteIndex(0x2c);
}

/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(u16 x, u16 y)
{
	Lcd_SetRegion(x, y, x, y);
}

/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�������
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x, u16 y, u16 Data)
{
	Lcd_SetRegion(x, y, x + 1, y + 1);
	LCD_WriteData_16Bit(Data);
	//	printf("X: %d Y: %d\r\n",x,y);
}

/*****************************************
 �������ܣ���TFTĳһ�����ɫ                          
 ���ڲ�����color  ����ɫֵ                                 
******************************************/
unsigned int Lcd_ReadPoint(u16 x, u16 y)
{
	unsigned int Data;
	Lcd_SetXY(x, y);

	//Lcd_ReadData();//���������ֽ�
	//Data=Lcd_ReadData();
	Lcd_WriteData(Data);
	return Data;
}
/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(u16 Color)
{
	unsigned int i, m;
	Lcd_SetRegion(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1);
	Lcd_WriteIndex(0x2C);
	for (i = 0; i < X_MAX_PIXEL; i++)
		for (m = 0; m < Y_MAX_PIXEL; m++)
		{
			LCD_WriteData_16Bit(Color);
		}
}

void showimageALL(const unsigned char *p) //��ʾ����ͼƬ
{
	int i;
	unsigned char picH, picL;
	Lcd_SetRegion(0, 0, 159, 79); //��������
	for (i = 0; i < 160 * 80; i++)
	{
		picL = *(p + i * 2); //���ݵ�λ��ǰ
		picH = *(p + i * 2 + 1);
		LCD_WriteData_16Bit(picH << 8 | picL);
	}
}
void showimage1(const unsigned char *p, int x, int y, int x1, int y1) //��ʾͼƬ
{
	int i;
	unsigned char picH, picL;
	Lcd_SetRegion(x, y, x1 - 1, y1 - 1); //��������
	for (i = 0; i < (x1 - x) * (y1 - y); i++)
	{
		picL = *(p + i * 2); //���ݵ�λ��ǰ
		picH = *(p + i * 2 + 1);
		LCD_WriteData_16Bit(picH << 8 | picL);
	}
}
//���ƻ��ߺ���
void Gui_DrawLinee(double x, double y, double x1, double y1, u16 Data)
{
	double k, b, i, n;
	int a;
	k = (y1 - y) / (x1 - x);
	b = y - k * x;
	n = k * i + b;
	for (i = x; i <= x1; i++)
	{
		a = k * i + b;
		//	printf("%f %d\r\n",n,a);
		Lcd_SetRegion(i, a, i + 1, a + 1);
		LCD_WriteData_16Bit(Data);
	}
}