#include "stm32f10x.h"
#include "spi_driver.h"

static void SPI_RCC_Configuration(SPI_TypeDef *SPIx)
{
	if (SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	}
}
/**
  * @brief  配置指定SPI的引脚
  * @param  SPIx 需要使用的SPI
  * @retval None
  */
static void SPI_GPIO_Configuration(SPI_TypeDef *SPIx) //D哥的板子用PB13，PB14，PB15
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (SPIx == SPI1)
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		//初始化片选输出引脚
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
	}
	else
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		//初始化片选输出引脚
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
}
/**
  * @brief  根据外部SPI设备配置SPI相关参数
  * @param  SPIx 需要使用的SPI
  * @retval None
  */
void SPI_Configuration(SPI_TypeDef *SPIx)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI_RCC_Configuration(SPIx);

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStruct);

	SPI_GPIO_Configuration(SPIx);

	SPI_SSOutputCmd(SPIx, ENABLE);
	SPI_Cmd(SPIx, ENABLE);
}
/**
  * @brief  写1字节数据到SPI总线
  * @param  SPIx 需要使用的SPI
  * @param  TxData 写到总线的数据
  * @retval 数据发送状态
  *		@arg 0 数据发送成功
  * 	@arg -1 数据发送失败
  */
int32_t SPI_WriteByte(SPI_TypeDef *SPIx, uint16_t TxData)
{
	uint8_t retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_TXE) == 0)
		; //等待发送区空
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	SPIx->DR = TxData; //发送一个byte
	retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == 0)
		; //等待接收完一个byte
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	SPIx->DR;
	return 0; //返回收到的数据
}
/**
  * @brief  从SPI总线读取1字节数据
  * @param  SPIx 需要使用的SPI
  * @param  p_RxData 数据储存地址
  * @retval 数据读取状态
  *		@arg 0 数据读取成功
  * 	@arg -1 数据读取失败
  */
int32_t SPI_ReadByte(SPI_TypeDef *SPIx, uint16_t *p_RxData)
{
	uint8_t retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_TXE) == 0)
		; //等待发送区空
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	SPIx->DR = 0xFF; //发送一个byte
	retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == 0)
		; //等待接收完一个byte
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	*p_RxData = SPIx->DR;
	return 0; //返回收到的数据
}
/**
  * @brief  向SPI总线写多字节数据
  * @param  SPIx 需要使用的SPI
  * @param  p_TxData 发送数据缓冲区首地址
  * @param	sendDataNum 发送数据字节数
  * @retval 数据发送状态
  *		@arg 0 数据发送成功
  * 	@arg -1 数据发送失败
  */
int32_t SPI_WriteNBytes(SPI_TypeDef *SPIx, uint8_t *p_TxData, uint32_t sendDataNum)
{
	uint8_t retry = 0;
	while (sendDataNum--)
	{
		while ((SPIx->SR & SPI_I2S_FLAG_TXE) == 0)
			; //等待发送区空
		{
			retry++;
			if (retry > 20000)
				return -1;
		}
		SPIx->DR = *p_TxData++; //发送一个byte
		retry = 0;
		while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == 0)
			; //等待接收完一个byte
		{
			SPIx->SR = SPIx->SR;
			retry++;
			if (retry > 20000)
				return -1;
		}
		SPIx->DR;
	}
	return 0;
}
/**
  * @brief  从SPI总线读取多字节数据
  * @param  SPIx 需要使用的SPI
  * @param  p_RxData 数据储存地址
  * @param	readDataNum 读取数据字节数
  * @retval 数据读取状态
  *		@arg 0 数据读取成功
  * 	@arg -1 数据读取失败
  */
int32_t SPI_ReadNBytes(SPI_TypeDef *SPIx, uint8_t *p_RxData, uint32_t readDataNum)
{
	uint8_t retry = 0;
	while (readDataNum--)
	{
		SPIx->DR = 0xFF;
		while (!(SPIx->SR & SPI_I2S_FLAG_TXE))
		{
			retry++;
			if (retry > 20000)
				return -1;
		}
		retry = 0;
		while (!(SPIx->SR & SPI_I2S_FLAG_RXNE))
		{
			retry++;
			if (retry > 20000)
				return -1;
		}
		*p_RxData++ = SPIx->DR;
	}
	return 0;
}
void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //PORTB时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //SPI2时钟使能

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //PB13/14/15复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIOB

	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15); //PB13/14/15上拉

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	 //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						 //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					 //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							 //串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						 //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					 //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;							 //CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);									 //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	SPI_Cmd(SPI2, ENABLE); //使能SPI外设

	SPI2_ReadWriteByte(0xff); //启动传输
}
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频
//SPI_BaudRatePrescaler_8   8分频
//SPI_BaudRatePrescaler_16  16分频
//SPI_BaudRatePrescaler_256 256分频

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1 &= 0XFFC7;
	SPI2->CR1 |= SPI_BaudRatePrescaler; //设置SPI2速度
	SPI_Cmd(SPI2, ENABLE);
}

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{
	u8 retry = 0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if (retry > 200)
			return 0;
	}
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
	retry = 0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if (retry > 200)
			return 0;
	}
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据
}

/*********************************END OF FILE**********************************/
