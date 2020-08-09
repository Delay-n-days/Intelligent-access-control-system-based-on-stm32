#include "main.h"
uint8_t Card_Type1[2];
uint8_t Card_ID[4];
uint8_t Card_KEY[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //{0x11,0x11,0x11,0x11,0x11,0x11};   //密码
uint8_t Card_Data[16];
uint8_t User_Card[10][4] = {0x29, 0x43, 0x1c, 0xbd,//当前库中 IC卡ID1,2,3
							0xb9, 0x6b, 0xff, 0xc1,
							0xfa, 0x10, 0x41, 0xea};
uint8_t status;
typedef struct USER
{
	u8 name[10];
	u8 age[2];
	u8 sex[2];
	u8 num[14];
	u8 Id[8];
	u8 addr[30];
} USE;

int main(void)
{
	int key, 	//按键状态
		flag,	//注册卡判断标志，flag=0;卡不在库中，flag=11，当前用户序号为1（root)卡为0;
		i,		//循环
		j, 		//循环
		root, 	//root卡刷卡次数
		rootkey, //root模式判断
		n, 		//当前用户序号
		a, 		//NUll
		doorstatu, //当前门状态
		mode, 	//当前模式（基础模式）(命令行控制模式)
		len, 	//当前超声波检测阈值
		t, 		//串口接收长度计数
		action, //当前串口执行的动作
		p, 		//当前串口输入的用户序号
		Len;	//当前串口输入的超声波检测阈值
	float length;//超声波得到的距离
	char id[17];//ID显示寄存器，用于Sprintf
	char aa[10];//各种数据显示寄存器，用于Sprintf
	USE User[2];//用户结构体数组初始化，暂时不用
	u8 UserCard[4];//用户ID读取寄存器
	u8 command[9];//命令暂存寄存器，用于串口返回命令
	//u8 time[16]="2020051100580001";//修改时间时使用的字符串
	flag = 1;	   //注册卡判断寄存器
	root = 0;	   //主卡连续刷的次数
	rootkey = 0;   //root mode标志位
	doorstatu = 0; //初始门状态为关闭
	mode = 0;	   //初始模式为检测卡模式
	action = 0;	   //串口初始模式为命令模式
	Len = R_SaveNumC();//从flash中读取检测阈值
	Card_Type1[0] = 0x04;//卡类型1
	Card_Type1[1] = 0x00;//卡类型2

	delay_init();									//延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);								//串口初始化为115200
	printf("\r\n****SERIAL PORT TEST****\r\n");		//串口测试

	DS1302_Init();									//实时时钟模块初始化
	SystemInit(); //System init.					//系统时钟初始化
	RC522_IO_Init();								//RFID读卡器IO初始化	
	BEEP_Init();									//蜂鸣器初始化
	LED_Init();										//LED I/O初始化
	DIANJI_Init();									//步进电机初始化
	KEY_Init();										//按键初始化
	Hcsr05Init();									//超声波模块初始化
	Lcd_Init();										//显示屏初始化（ST7735 0.96)
	W25QXX_Init(); 									//W25QXX初始化
	//W25QXX_Erase_Chip();							//整片擦除
	//Change_SaveNumB(1,0);							//清除日志
	LCD_BL_SET;										//lcdI/O启动
	LCD_CS_CLR;										//拉低片选
	showimageALL(gImage_2);							//显示启动图片
	delay_ms(1000);
	Lcd_Clear(WHITE);								//清屏为白色							    	
	//DS1302_ChangeTime(time);						//修改系统时间

	PcdReset();		 								//复位RC522
	PcdAntennaOff(); 								//关闭天线
	delay_ms(100);
	PcdAntennaOn(); 								//开启天线
	//doorstatu=Door_control(0);
	printf("\r\n****HJT SERIAL PORT TEST****\r\n");

	Usermessage_Add(0, "ROOT", "NU", "NU", "NULL", User_Card[0], "NULL");//将ROOT（主卡）信息写入Flash
	sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
	Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);//输出当前用户数和日志数并打印在显示屏  

	while (1)
	{
		key = KEY_Scan(0);			//按键是否按下
		if (mode == 0)				//判断模式
		{
			if (rootkey == 0)		//判断是否为root模式
			Gui_DrawFont_GBK16(144, 00, BLACK, WBLUE, "  ");//root状态打印显示，非root模式状态下为空
			length = Hcsr05GetLength(); //得到距离
			if (length <= Len)//根据门状态判断是报警还是或者自动关闭
			{
				if (doorstatu == 0)
				{
					ledout(0);
					beepout(0);
				}
				if (doorstatu == 1)
				{
					ledout(11);
					BEEP_once();
					doorstatu = Door_control(0);//关门
					root = 0;
					rootkey = 0;
				}
			}
			if (length > 700)//防止超声波模块出错
				length = -1;

			DS1302_GetTime();//获取当前时间

			sprintf(aa, "%3.0f", length);//输出当前距离
			Gui_DrawFont_GBK16(132, 64, BLUE, WHITE, (u8 *)aa); //屏幕打印距离

			key = KEY_Scan(0);
			delay_ms(10);
			if (MI_OK == PcdRequest(0x26, Card_Type1)) //寻卡函数，如果成功返回MI_OK  打印一次卡号
			{
				//printf("ID:%02X%02X%02X%02X",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);

				status = PcdAnticoll(Card_ID); //防冲撞 如果成功返回MI_OK
				if (status != MI_OK)
				{
					//printf("Anticoll Error\r\n");
				}
				else
				{
					a = rootkey;
					sprintf(id, "ID:%02X%02X%02X%02X  ", Card_ID[0], Card_ID[1], Card_ID[2], Card_ID[3]); //读取卡号
					rootkey = a;
					Gui_DrawFont_GBK16(16, 16, BLUE, WHITE, (u8 *)id);
					for (i = 0; i < R_SaveNumA(); i++) //与库中卡号做对比
					{
						GetUserID(i, UserCard);
						for (j = 0; j < 4; j++)
						{
							if (Card_ID[j] == UserCard[j])
								flag = (i + 1) * 10 + 1; //利用flag可以读取到卡的序号
							else
								flag = 0;
							break;
						}
						if (flag != 0) //flag不等0判断是否是主卡，然后跳出
						{
							if (i == 0) //检测IC卡是否为主卡
								root = root + 1;
							if (i != 0)
								root = 0;
							break;
						}
					}

					status = PcdSelect(Card_ID); //选卡 如果成功返回MI_OK
					if (status != MI_OK)
					{
						printf("Select Card Error\r\n");
					}
					else
					{
						BEEP_once(); //蜂鸣器响一下
									 //printf("Select Card OK\r\n");
					}

					if (root >= 2) //主卡连续刷超过两次，进入管理员模式
					{
						rootkey = 1;//进入root模式
						Gui_DrawFont_GBK16(144, 00, WHITE, BLACK, "锕");//一个自己画的管理员模式图标取模
					}

					status = PcdHalt(); //卡片进入休眠状态防止重复读卡
					if (status != MI_OK)
					{
						printf("PcdHalt Error\r\n");
					}
					else
					{
						if (rootkey == 1) //如果已经进入了root模式
						{
							if (flag != 0 && flag != 11) //如果卡在库中，且不是主卡
							{
								n = (flag - 1) / 10 - 1; //得到卡的序号，也就是数组的行标
								DelUser(n);				 //删除该用户信息
								sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
								Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
								printf("n=%d  flag=%d %d\n", n, flag, (flag - 1) % 10 - 1);
								rootkey = 0;
								Gui_DrawFont_GBK16(0, 32, BLACK, WHITE, "DEL √ ");
							}
							else if (flag == 0) //卡不在库中
							{
								AddUser(Card_ID); //添加一个只有卡号的用户信息
								sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
								Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
								rootkey = 0;
								Gui_DrawFont_GBK16(0, 32, BLACK, WHITE, "ADD √ ");
							}
						}
						else
						{
							if (flag != 0) //检测卡号是否正确
							{
								Gui_DrawFont_GBK16(0, 16, BLUE, WHITE, "√");
								Log_save(flag);
								sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
								Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
								ledout(1);
								beepout(1);
								if (doorstatu == 0)
								{
									ledout(10);
									doorstatu = Door_control(1);
								}
							}

							else //
							{
								Gui_DrawFont_GBK16(0, 16, RED, WHITE, "×");
								if (doorstatu == 1)
								{
									ledout(11);
									doorstatu = Door_control(1);
								}
							}
						}
					}
				}
			}
		}
		if (mode)//串口命令模式
		{
			if (USART_RX_STA & 0x8000 && action == 0)
			{
				len = USART_RX_STA & 0x3f; //得到此次接收到的数据长度
				if (len > 9 | len < 9)
				{
					printf("请输入正确的命令...\n");
					USART_RX_STA = 0;
				}
				else
				{
					printf("\r\n Your message is:\r\n\r\n");
					for (t = 0; t < 9; t++)
					{
						USART_SendData(USART1, USART_RX_BUF[t]);
						//向串口 1 发送数据
						command[t] = USART_RX_BUF[t];
						while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
							; //等待//发送结束
					}
					printf("\r\n\r\n"); //插入换行

					if (strcmp((const char *)command, "HELP11111") == 0)
					{
						printf("CHANGEMES:修改用户信息\n"); //帮助
						printf("CHANGELEN:修改检测阈值\n");
						printf("RETURNUSE:返回用户信息\n");
						printf("CLEARUSE1:清空用户信息\n");
						printf("RETURNLOG:返回日志\n");
						printf("CLEARLOG1:清空日志\n");
						printf("CHANGETIM:修改时间\n");
						printf("OPENDOOR1:开门\n");
						printf("CLOSEDOOR:关门\n");
						printf("ESC111111:退出命令模式\n");
						printf("END...\n请输入命令:\n");
					}
					else if (strcmp((const char *)command, "CHANGEMES") == 0)
					{
						action = 1; //修改用户信息
						UserMessage_display();
						printf("请输入您要修改的用户编号:\n");
						USART_RX_STA = 0;
					}
					else if (strcmp((const char *)command, "CHANGELEN") == 0)
					{
						action = 7; //修改检测阈值
						printf("当前检测阈值为: %d\n", R_SaveNumC());
						printf("请输入您修改后的检测阈值:\n");
						USART_RX_STA = 0;
					}
					else if (strcmp((const char *)command, "CLEARLOG1") == 0)
					{
						Change_SaveNumB(1, 0);
						printf("已清空日志...\n请输入命令:\n"); //清空日志
						sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
						Gui_DrawFont_GBK16(0, 64, WHITE, BLACK, (u8 *)aa);
					}
					else if (strcmp((const char *)command, "CLEARUSE1") == 0)
					{
						Change_SaveNumA(1, 1);
						printf("已清空用户信息...\n请输入命令:\n"); //清空日志
						sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
						Gui_DrawFont_GBK16(0, 64, WHITE, BLACK, (u8 *)aa);
					}
					else if (strcmp((const char *)command, "CHANGETIM") == 0)
					{
						action = 8; //修改时间
						printf("请输入时间(实例:2020010112000001):\n");
						USART_RX_STA = 0;
					}
					else if (strcmp((const char *)command, "OPENDOOR1") == 0)
					{
						if (doorstatu == 0)
							doorstatu = Door_control(1);
						USART_RX_STA = 0;
						printf("执行完毕！\n请输入命令:\n");
					} //开门
					else if (strcmp((const char *)command, "CLOSEDOOR") == 0)
					{
						if (doorstatu == 1)
							doorstatu = Door_control(0);
						USART_RX_STA = 0;
						printf("执行完毕！\n请输入命令:\n");
					} //关门
					else if (strcmp((const char *)command, "ESC111111") == 0)
					{
						mode = 0;
						printf("已退出串口命令模式:\n");
						rootkey = 0;
						Lcd_Clear(WHITE);
						sprintf(aa, "USER:%d LOG:%d\r\n", R_SaveNumA(), R_SaveNumB());
						Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
						USART_RX_STA = 0;
					} //退出串口调试模式
					else if (strcmp((const char *)command, "RETURNLOG") == 0)
					{
						Log_display();
						USART_RX_STA = 0;
						printf("请输入命令:\n");
					} //返回日志
					else if (strcmp((const char *)command, "RETURNUSE") == 0)
					{
						UserMessage_display();
						USART_RX_STA = 0;
						printf("请输入命令:\n");
					} //返回用户信息
					else if (strcmp((const char *)command, "SHOWMYJPG") == 0)
					{
						showimageALL(gImage_2);
						printf("请输入命令:\n");
					} //返回用户信息

					else
					{
						printf("找不到命令...\n");
						USART_RX_STA = 0;
					}
					USART_RX_STA = 0;
				}
			}

			else if (USART_RX_STA & 0x8000 && action == 1)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的用户编号为:");
				for (t = 0; t < len; t++)
				{
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				p = atoi((char *)USART_RX_BUF);
				printf("\n");
				if (p >= R_SaveNumA())
				{
					printf("编号输入有误\n请输入从新输入编号:\n");
					action = 1;
					USART_RX_STA = 0;
				}
				else
				{
					action = 2;
					USART_RX_STA = 0;
					printf("请输入用户姓名:\n");
				}
			}
			else if (USART_RX_STA & 0x8000 && action == 2)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的用户姓名为:\n");
				for (t = 0; t < len; t++)
				{
					User[0].name[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("请输入用户性别:\n");
				action = 3;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 3)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的用户性别为:\n");
				for (t = 0; t < len; t++)
				{
					User[0].sex[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("请输入用户年龄:\n");
				action = 4;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 4)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的用户年龄为:\n");
				for (t = 0; t < len; t++)
				{
					User[0].age[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("请输入用户电话:\n");
				action = 5;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 5)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的用户电话为:\n");
				for (t = 0; t < len; t++)
				{
					User[0].num[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("请输入用户住址:\n");
				action = 6;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 6)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的用户住址为:\n");
				for (t = 0; t < len; t++)
				{
					User[0].addr[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				GetUserID(p, User_Card[4]);
				Usermessage_Add(p, User[0].name, User[0].age, User[0].sex, User[0].num, User_Card[4], User[0].addr);
				printf("-----------信息录入完成--------\n请输入命令:\n");
				action = 0;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 7)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的检测阈值为:\n");
				for (t = 0; t < len; t++)
				{
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				p = atoi((char *)USART_RX_BUF);
				printf("\n");
				if (p >= 750)
				{
					printf("编号阈值有误\n请输入重新输入阈值(MAX=750):\n");
					action = 7;
					USART_RX_STA = 0;
				}
				else
				{
					Change_SaveNumC(1, p);
					Len = R_SaveNumC();
					action = 0;
					USART_RX_STA = 0;
					printf("-----------阈值修改完成--------\n请输入命令:\n");
				}
			}
			else if (USART_RX_STA & 0x8000 && action == 8)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n 你输入的时间为:\n");
				for (t = 0; t < len; t++)
				{
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				DS1302_ChangeTime(USART_RX_BUF);
				printf("\n");
				action = 0;
				USART_RX_STA = 0;
				printf("-----------时间修改完成--------\n请输入命令:\n");
			}
			else
			{
				ledout(1);
				ledout(10);
			}
		}

		if (key)
		{
			root = 0;
			BEEP_once();

			if (rootkey & mode == 0) //如果进入了root模式
			{
				Lcd_Clear(WHITE);
				showimage1(gImage_3, 40, 0, 120, 45);
				mode = 1; //进入串口调试模式

				Gui_DrawFont_GBK16(0, 48, BLACK, WHITE, "*已进入串口命令模式*");
				Gui_DrawFont_GBK16(0, 64, BLACK, WHITE, "*请在串口上位机操作*");
				printf("已进入串口调试模式，请输入命令......\n");
			}
			else if (mode == 0)
			{
				ledout(1);
				ledout(10);
				Log_display();
				Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, "LOG Send OK! ");
			}
			else if (mode == 1)
			{
				mode = 0;
				rootkey = 0;
				Lcd_Clear(WHITE);
				sprintf(aa, "USER:%d LOG:%d\r\n", R_SaveNumA(), R_SaveNumB());
				Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
			}
		}
	}
}
