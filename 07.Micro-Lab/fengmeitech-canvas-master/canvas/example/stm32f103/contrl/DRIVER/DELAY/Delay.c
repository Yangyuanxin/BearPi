/******************************************************************
 * 文件：Delay.c
 * 功能：实现延时函数
 * 日期：2018-02-22
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "DELAY/Delay.h"


/**
 * 功能：初始化Systick定时器
 * 参数：None
 * 返回值：None
 */
void initSysTick(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //设置时钟源8分频
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;            //使能中断
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;             //开定时器
    SysTick->LOAD = 9;                                    //随意设置一个重装载值
}


/**
 * 功能：us级别延时
 * 参数：xus：要延时的时间
 * 返回值：None
 */
void Delay_us(u32 xus)
{
    SysTick->LOAD = 9 * xus; //计9次为1us，xus则重装载值要*9
    SysTick->VAL = 0;        //计数器归零
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); //等待计数完成
}

/**
 * 功能：ms级别延时
 * 参数：xms：要延时的时间
 * 返回值：None
 */
void Delay_ms(u32 xms)
{
    SysTick->LOAD = 9000; //计9次为1us，1000次为1ms
    SysTick->VAL = 0;     //计数器归零
    while (xms--)
    {
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); //等待单次计数完成
    }
}