/******************************************************************
 * 文件：SPIx.h
 * 功能：声明SPI功能函数
 * 日期：2018-03-17
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"

void initSPIx(SPI_TypeDef* SPIx);
void setNSS(u8 status);
void sendSPIxByte(SPI_TypeDef* SPIx, u16 byte);
u16 readSPIxByte(SPI_TypeDef* SPIx);

#endif
