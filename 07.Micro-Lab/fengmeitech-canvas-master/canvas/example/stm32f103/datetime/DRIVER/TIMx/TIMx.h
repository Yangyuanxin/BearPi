/******************************************************************
 * 文件：TIMx.c
 * 功能：声明TIM3相关函数
 * 日期：2018-03-08
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __TIM3_H
#define __TIM3_H
#include "stm32f10x_tim.h"

void initTIMx(TIM_TypeDef* TIMx,u16 prescaler,u16 period,u16 IT_Source,FunctionalState NewState);
void initTIM3OC1(u16 duty);
void initTIM4OC1(u16 duty);
void initTIM4OC2(u16 duty);
void initTIM4IC3(u16 ICPolarity);
void initTIM2OC1(u16 duty);
void initTIM2OC2(u16 duty);
void setPeriod(TIM_TypeDef* TIMx,u16 period);
void setPrescaler(TIM_TypeDef* TIMx,u16 prescaler);
u16 getPeriod(TIM_TypeDef* TIMx);

#endif