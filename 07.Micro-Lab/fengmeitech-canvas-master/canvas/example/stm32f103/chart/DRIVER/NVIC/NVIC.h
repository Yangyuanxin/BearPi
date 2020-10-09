/******************************************************************
 * 文件：NVIC.h
 * 功能：声明NVIC相关函数
 * 日期：2018-02-26
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __NVIC_H
#define __NVIC_H
#include "stm32f10x.h"

void initNVIC(u32 NVIC_PriorityGroup);
void setNVIC(u8 InterruptNum, u8 PreemptionPriority, u8 SubPriority, FunctionalState NVIC_Sta);

#endif
