/******************************************************************
 * 文件：Light.h
 * 功能：声明Light相关函数
 * 日期：2018-03-14
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __LIGHT_H
#define __LIGHT_H
#include "stm32f10x.h"

void initLight(u8 brightness,u8 colortemp,u8 red,u8 green,u8 blue);
void setLight(u8 brightness,u8 colortemp);
void setRGB(u8 red,u8 green,u8 blue);

#endif
