#ifndef __SPI_DRIVER_H
#define __SPI_DRIVER_H

#include "stm32f10x.h"

void SPI_Configuration(SPI_TypeDef *SPIx);
int32_t SPI_WriteNBytes(SPI_TypeDef *SPIx, uint8_t *p_TxData, uint32_t sendDataNum);
int32_t SPI_ReadNBytes(SPI_TypeDef *SPIx, uint8_t *p_RxData, uint32_t readDataNum);
int32_t SPI_WriteByte(SPI_TypeDef *SPIx, uint16_t TxData);
int32_t SPI_ReadByte(SPI_TypeDef *SPIx, uint16_t *p_RxData);
void SPI2_Init(void);             //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet);  //设置SPI速度
u8 SPI2_ReadWriteByte(u8 TxData); //SPI总线读写一个字节
#endif
