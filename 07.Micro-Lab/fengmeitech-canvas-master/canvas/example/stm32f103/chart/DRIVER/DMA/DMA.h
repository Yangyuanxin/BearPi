/******************************************************************
 * 文件：DMA.c
 * 功能：声明DMA相关函数
 * 日期：2018-03-15
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "stm32f10x.h"

void initDMA(DMA_Channel_TypeDef* DMA_Chx, u32 periph_addr, u32 memory_addr, u16 trans_size);