/******************************************************************
 * 文件：NVIC.c
 * 功能：实现NVIC相关函数
 * 日期：2018-02-26
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "NVIC/NVIC.h"

/**
 * 功能：初始化NVIC，初始化内容包括：分配优先级分组号，分配对应中断优先级
 * 参数：NVIC_PriorityGroup : 设置优先级分组号
 *      参数值：NVIC_PriorityGroup_x
 * 返回值：None
 */
void initNVIC(u32 NVIC_PriorityGroup)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup); //中断分组
    setNVIC(USART1_IRQn,0,1,ENABLE);              //使能UART1总中断
}


/**
 * 功能：设置对应外设中断
 * 参数：InterruptNum: 对应中断的中断编号
 *       PreemptionPriority:指定抢占优先级
 *       SubPriority:指定子优先级
 *       NVIC_Sta:使能或者失能中断
 * 返回值：None
 */  
void setNVIC(u8 InterruptNum, u8 PreemptionPriority, u8 SubPriority, FunctionalState NVIC_Sta)
{
    NVIC_InitTypeDef NVIC_InitStructure;                                       //定义NVIC初始化结构体

    NVIC_InitStructure.NVIC_IRQChannel = InterruptNum;                         //指定配置的中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority; //设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;               //设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = NVIC_Sta;                          //使/失能中断
    NVIC_Init(&NVIC_InitStructure);                                            //设置生效
}
