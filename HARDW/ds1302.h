#ifndef ds18B20_H
#define ds18B20_H
#include "stm32f10x.h"
#include "sys.h"
#include "Lcd_Driver.h"
//DS1302引脚定义,可根据实际情况自行修改端口定义
#define DS1302_OutPut_Mode() {GPIO_InitTypeDef  GPIO_InitStructure;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;GPIO_Init(GPIOB, &GPIO_InitStructure);	}
#define DS1302_InPut_Mode()  {GPIO_InitTypeDef  GPIO_InitStructure;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;GPIO_Init(GPIOB, &GPIO_InitStructure);}
 
#define DS1302_IN  PBin(9)
#define DS1302_OUT PBout(9)
#define DS1302_RST PBout(3)
#define DS1302_CLK PBout(8)
 
struct TIMEData
{
	u16 year;
	u8  month;
	u8  day;
	u8  hour;
	u8  minute;
	u8  second;
	u8  week;
};
extern struct TIMEData TimeData;
extern u8 readtime[15];
void DS1302_Init(void);
void DS1302_WriteByte(u8 addr,u8 data);
u8   DS1302_ReadByte(u8 addr);
void DS1302_WriteTime(void);
void DS1302_ReadTime(void);
void DS1302_GetTime(void);
int GetTimeyear(void);
int GetTimemonth(void);
int GetTimeday(void);
int GetTimehour(void);
int GetTimeminute(void);
int GetTimesecond(void);
int GetTimeweek(void);
void DS1302_ChangeTime(u8 * a);
void fun(u8 *s);
#endif

