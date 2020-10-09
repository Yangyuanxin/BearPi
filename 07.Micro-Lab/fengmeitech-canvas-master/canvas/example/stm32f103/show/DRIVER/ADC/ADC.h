/******************************************************************
 * 文件：ADC.c
 * 功能：声明ADC驱动功能函数
 * 日期：2018-03-1
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h"

void initADC(void);
u16 getConvValue(void);
u16 getConvValueAve(u8 counts,u32 xus);

#endif
