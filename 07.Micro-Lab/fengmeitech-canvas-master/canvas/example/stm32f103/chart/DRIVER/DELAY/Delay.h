/******************************************************************
 * 文件：Delay.h
 * 功能：声明延时函数
 * 日期：2018-02-22
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "stm32f10x.h"

void initSysTick(void);
void Delay_us(u32 xus);
void Delay_ms(u32 xms);
