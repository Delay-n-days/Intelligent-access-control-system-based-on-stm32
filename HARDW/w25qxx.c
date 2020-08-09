#include "w25qxx.h"
#include "spi_driver.h"
#include "delay.h"
#include "usart.h"
#include "stdlib.h"
#include "ds1302.h"

u16 W25QXX_TYPE = W25Q64; //默认是W25Q64

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector

//初始化SPI FLASH的IO口
void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //PORTB时钟使能

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		 // PB12 推挽
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);

	W25QXX_CS = 1;							//SPI FLASH不选中
	SPI2_Init();							//初始化SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2); //设置为18M时钟,高速模式
	W25QXX_TYPE = W25QXX_ReadID();			//读取FLASH ID.
}

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 W25QXX_ReadSR(void)
{
	u8 byte = 0;
	W25QXX_CS = 0;							//使能器件
	SPI2_ReadWriteByte(W25X_ReadStatusReg); //发送读取状态寄存器命令
	byte = SPI2_ReadWriteByte(0Xff);		//读取一个字节
	W25QXX_CS = 1;							//取消片选
	return byte;
}
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_Write_SR(u8 sr)
{
	W25QXX_CS = 0;							 //使能器件
	SPI2_ReadWriteByte(W25X_WriteStatusReg); //发送写取状态寄存器命令
	SPI2_ReadWriteByte(sr);					 //写入一个字节
	W25QXX_CS = 1;							 //取消片选
}
//W25QXX写使能
//将WEL置位
void W25QXX_Write_Enable(void)
{
	W25QXX_CS = 0;						  //使能器件
	SPI2_ReadWriteByte(W25X_WriteEnable); //发送写使能
	W25QXX_CS = 1;						  //取消片选
}
//W25QXX写禁止
//将WEL清零
void W25QXX_Write_Disable(void)
{
	W25QXX_CS = 0;						   //使能器件
	SPI2_ReadWriteByte(W25X_WriteDisable); //发送写禁止指令
	W25QXX_CS = 1;						   //取消片选
}
//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;
	W25QXX_CS = 0;
	SPI2_ReadWriteByte(0x90); //发送读取ID命令
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	Temp |= SPI2_ReadWriteByte(0xFF) << 8;
	Temp |= SPI2_ReadWriteByte(0xFF);
	W25QXX_CS = 1;
	return Temp;
}
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(u8 *pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	u16 i;
	W25QXX_CS = 0;								//使能器件
	SPI2_ReadWriteByte(W25X_ReadData);			//发送读取命令
	SPI2_ReadWriteByte((u8)((ReadAddr) >> 16)); //发送24bit地址
	SPI2_ReadWriteByte((u8)((ReadAddr) >> 8));
	SPI2_ReadWriteByte((u8)ReadAddr);
	for (i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = SPI2_ReadWriteByte(0XFF); //循环读数
	}
	W25QXX_CS = 1;
}
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25QXX_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 i;
	W25QXX_Write_Enable();						 //SET WEL
	W25QXX_CS = 0;								 //使能器件
	SPI2_ReadWriteByte(W25X_PageProgram);		 //发送写页命令
	SPI2_ReadWriteByte((u8)((WriteAddr) >> 16)); //发送24bit地址
	SPI2_ReadWriteByte((u8)((WriteAddr) >> 8));
	SPI2_ReadWriteByte((u8)WriteAddr);
	for (i = 0; i < NumByteToWrite; i++)
		SPI2_ReadWriteByte(pBuffer[i]); //循环写数
	W25QXX_CS = 1;						//取消片选
	W25QXX_Wait_Busy();					//等待写入结束
}
//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 pageremain;
	pageremain = 256 - WriteAddr % 256; //单页剩余的字节数
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite; //不大于256个字节
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; //写入结束了
		else	   //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain; //减去已经写入了的字节数
			if (NumByteToWrite > 256)
				pageremain = 256; //一次可以写入256个字节
			else
				pageremain = NumByteToWrite; //不够256个字节了
		}
	};
}
//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u8 *W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096; //扇区地址
	secoff = WriteAddr % 4096; //在扇区内的偏移
	secremain = 4096 - secoff; //扇区剩余空间大小
	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
	if (NumByteToWrite <= secremain)
		secremain = NumByteToWrite; //不大于4096个字节
	while (1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); //读出整个扇区的内容
		for (i = 0; i < secremain; i++)				  //校验数据
		{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break; //需要擦除
		}
		if (i < secremain) //需要擦除
		{
			W25QXX_Erase_Sector(secpos);	//擦除这个扇区
			for (i = 0; i < secremain; i++) //复制
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //写入整个扇区
		}
		else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //写已经擦除了的,直接写入扇区剩余区间.
		if (NumByteToWrite == secremain)
			break; //写入结束了
		else	   //写入未结束
		{
			secpos++;	//扇区地址增1
			secoff = 0; //偏移位置为0

			pBuffer += secremain;		 //指针偏移
			WriteAddr += secremain;		 //写地址偏移
			NumByteToWrite -= secremain; //字节数递减
			if (NumByteToWrite > 4096)
				secremain = 4096; //下一个扇区还是写不完
			else
				secremain = NumByteToWrite; //下一个扇区可以写完了
		}
	};
}
//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(void)
{
	int root;
	root = 96 * 16;
	while (root >= 0)
	{
		W25QXX_Erase_Sector(root);
		root--;
	}
}
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
	//监视falsh擦除情况,测试用
	//printf("fe:%x\r\n",Dst_Addr);
	Dst_Addr *= 4096;
	W25QXX_Write_Enable(); //SET WEL
	W25QXX_Wait_Busy();
	W25QXX_CS = 0;								//使能器件
	SPI2_ReadWriteByte(W25X_SectorErase);		//发送扇区擦除指令
	SPI2_ReadWriteByte((u8)((Dst_Addr) >> 16)); //发送24bit地址
	SPI2_ReadWriteByte((u8)((Dst_Addr) >> 8));
	SPI2_ReadWriteByte((u8)Dst_Addr);
	W25QXX_CS = 1;		//取消片选
	W25QXX_Wait_Busy(); //等待擦除完成
}
//等待空闲
void W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR() & 0x01) == 0x01)
		; // 等待BUSY位清空
}
//进入掉电模式
void W25QXX_PowerDown(void)
{
	W25QXX_CS = 0;						//使能器件
	SPI2_ReadWriteByte(W25X_PowerDown); //发送掉电命令
	W25QXX_CS = 1;						//取消片选
	delay_us(3);						//等待TPD
}
//唤醒
void W25QXX_WAKEUP(void)
{
	W25QXX_CS = 0;							   //使能器件
	SPI2_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB
	W25QXX_CS = 1;							   //取消片选
	delay_us(3);							   //等待TRES1
}
//保存一个用户信息
void Usermessage_Add(int i, u8 *name, u8 age[2], u8 sex[2], u8 num[14], u8 Id[8], u8 addr[30])
{
	W25QXX_Write(name, i * 68 + 0, 10);
	W25QXX_Write(num, i * 68 + 10, 20);
	W25QXX_Write(addr, i * 68 + 30, 30);
	W25QXX_Write(Id, i * 68 + 60, 4);
	W25QXX_Write(age, i * 68 + 64, 2);
	W25QXX_Write(sex, i * 68 + 66, 2);
	//printf("----Write Message OK!---一\n");
}
//输出一个用户信息
void Usermessage_Print(int i)
{
	u8 datatemp[30];
	u8 Nam[10];
	u8 Sex[2];
	printf("-------------USER-%02d-------------\n", i);
	W25QXX_Read(datatemp, i * 68 + 60, 4);
	printf("ID:%02X%02X%02X%02X\n", datatemp[0], datatemp[1], datatemp[2], datatemp[3]);
	W25QXX_Read(Nam, i * 68 + 0, 10);
	printf("姓名：%s\n", Nam);
	W25QXX_Read(Sex, i * 68 + 66, 2);
	printf("性别：%s\n", Sex);
	W25QXX_Read(datatemp, i * 68 + 64, 2);
	printf("年龄：%c%c\n", datatemp[0], datatemp[1]);
	W25QXX_Read(datatemp, i * 68 + 10, 20);
	printf("电话：%s\n", datatemp);
	W25QXX_Read(datatemp, i * 68 + 30, 30);
	printf("住址：%s\n", datatemp);
}
//获得用户ID
void GetUserID(int i, u8 *pBuffer)
{
	u8 datatemp[30];

	W25QXX_Read(datatemp, i * 68 + 60, 4);
	pBuffer[0] = datatemp[0];
	pBuffer[1] = datatemp[1];
	pBuffer[2] = datatemp[2];
	pBuffer[3] = datatemp[3];
}
//获得用户姓名
void GetUserName(int i, u8 *pBuffer)
{
	u8 datatemp[10];

	W25QXX_Read(datatemp, i * 68 + 0, 10);
	pBuffer[0] = datatemp[0];
	pBuffer[1] = datatemp[1];
	pBuffer[2] = datatemp[2];
	pBuffer[3] = datatemp[3];
	pBuffer[4] = datatemp[4];
	pBuffer[5] = datatemp[5];
	pBuffer[6] = datatemp[6];
	pBuffer[7] = datatemp[7];
	pBuffer[8] = datatemp[8];
	pBuffer[9] = datatemp[9];
}
//删除一个用户信息
void DelUser(int i)
{
	u8 datatemp[68];
	int n, j;
	n = R_SaveNumA();
	for (j = i + 1; j <= n; j++)
	{
		W25QXX_Read(datatemp, j * 68, 68);
		W25QXX_Write(datatemp, (j - 1) * 68, 68);
	}
	Change_SaveNumA(0, -1);
}
//增加一个只有卡号的用户信息
void AddUser(u8 *pBuffer)
{
	Usermessage_Add(R_SaveNumA(), "NewUser", "NU", "NU", "NULL", pBuffer, "NULL");
	Change_SaveNumA(0, 1);
}
//改变存储器中最后4位保存的数字
//mode:模式：mode=0时保存的数字+=a
//					 mode=1时保存的数字=a
void Change_SaveNumA(int mode, int a)
{
	int n;
	u8 datatemp[9];
	if (mode == 0)
	{
		W25QXX_Read(datatemp, 96 * 16 * 4096 - 5, 4);
		n = atoi((char *)datatemp);
		n = n + a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 5, 4);
	}
	else
	{
		n = a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 5, 4);
	}
}
//返回存储器中最后9位保存的数字
int R_SaveNumA(void)
{
	int n;
	u8 datatemp[5];
	W25QXX_Read(datatemp, 96 * 16 * 4096 - 5, 4);
	n = atoi((char *)datatemp);
	return n;
}
void Change_SaveNumB(int mode, int a)
{
	int n;
	u8 datatemp[5];
	if (mode == 0)
	{
		W25QXX_Read(datatemp, 96 * 16 * 4096 - 10, 5);
		n = atoi((char *)datatemp);
		n = n + a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 10, 5);
	}
	else
	{
		n = a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 10, 5);
	}
}
//返回存储器中最后位保存的数字
int R_SaveNumB(void)
{
	int n;
	u8 datatemp[5];
	W25QXX_Read(datatemp, 96 * 16 * 4096 - 10, 5);
	n = atoi((char *)datatemp);
	return n;
}
void Change_SaveNumC(int mode, int a)
{
	int n;
	u8 datatemp[5];
	if (mode == 0)
	{
		W25QXX_Read(datatemp, 96 * 16 * 4096 - 15, 5);
		n = atoi((char *)datatemp);
		n = n + a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 15, 5);
	}
	else
	{
		n = a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 15, 5);
	}
}
//返回存储器中最后位保存的数字
int R_SaveNumC(void)
{
	int n;
	u8 datatemp[5];
	W25QXX_Read(datatemp, 96 * 16 * 4096 - 15, 5);
	n = atoi((char *)datatemp);
	return n;
}
//输出当前日志
void Log_Print(int i)
{
	int size = 96 * 16 * 4096 - 45;
	u8 Nam[10];
	u8 ID[4];
	u8 year[4];
	u8 month[2];
	u8 day[2];
	u8 hour[2];
	u8 minute[2];
	u8 second[2];
	u8 week[1];
	printf("---------------------LOG-%02d----------------------\n", i);
	W25QXX_Read(Nam, size - 30 * i + 4, 10);
	printf("姓名:%s ", Nam);
	W25QXX_Read(ID, size - 30 * i, 4);
	printf("    [ID:%02X%02X%02X%02X]\n", ID[0], ID[1], ID[2], ID[3]);
	W25QXX_Read(year, size - 30 * i + 14, 4);
	printf("时间:%c%c%c%c-", year[0], year[1], year[2], year[3]);
	W25QXX_Read(month, size - 30 * i + 18, 2);
	printf("%c%c-", month[0], month[1]);
	W25QXX_Read(day, size - 30 * i + 20, 2);
	printf("%c%c ", day[0], day[1]);
	W25QXX_Read(hour, size - 30 * i + 22, 2);
	printf("%c%c:", hour[0], hour[1]);
	W25QXX_Read(minute, size - 30 * i + 24, 2);
	printf("%c%c:", minute[0], minute[1]);
	W25QXX_Read(second, size - 30 * i + 26, 2);
	printf("%c%c", second[0], second[1]);
	W25QXX_Read(week, size - 30 * i + 28, 1);
	printf(" 周:%c\n", week[0]);
}

void Log_save(int flag)
{
	u8 Log[30];
	u8 temp[10];
	int i, n, r;
	r = R_SaveNumB();
	GetUserID((flag - 1) / 10 - 1, temp);
	for (i = 0; i < 4; i++)
		Log[i] = temp[i];
	GetUserName((flag - 1) / 10 - 1, temp);
	for (i = 0; i < 10; i++)
		Log[i + 4] = temp[i];
	n = GetTimeyear();
	sprintf((char *)temp, "%4d", n);
	for (i = 0; i < 4; i++)
		Log[i + 14] = temp[i];
	n = GetTimemonth();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 18] = temp[i];
	n = GetTimeday();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 20] = temp[i];
	n = GetTimehour();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 22] = temp[i];
	n = GetTimeminute();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 24] = temp[i];
	n = GetTimesecond();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 26] = temp[i];
	n = GetTimeweek();
	sprintf((char *)temp, "%1d", n);
	for (i = 0; i < 1; i++)
		Log[i + 28] = temp[i];
	//for(i=0;i<29;i++)printf("%02X ",Log[i]);
	W25QXX_Write(Log, 96 * 16 * 4096 - 45 - r * 30, 30);
	printf("log save ok\n");
	Log_Print(R_SaveNumB());
	Change_SaveNumB(0, 1);
}
void Log_display(void)
{
	int i;
	printf("============日志============\n");
	for (i = 0; i < R_SaveNumB(); i++)
		Log_Print(i);
	printf("============日志============\n");
	printf("用户数:%d 日志条目:%d\n", R_SaveNumA(), R_SaveNumB());
}
void UserMessage_display(void)
{
	int i;
	printf("============用户============\n");
	for (i = 0; i < R_SaveNumA(); i++)
		Usermessage_Print(i);
	printf("===========用户============\n");
	printf("用户数:%d \n", R_SaveNumA());
}
