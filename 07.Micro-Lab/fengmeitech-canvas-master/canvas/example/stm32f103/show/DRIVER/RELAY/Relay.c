/******************************************************************
 * 文件：Delay.c
 * 功能：实现继电器函数
 * 日期：2018-03-11
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "RELAY/Relay.h"

/**
 * 功能：初始化继电器控制IO
 * 参数：None
 * 返回值：None
 */
void initRelay(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义GPIO初始化结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE); //使能GPIO和复用外设时钟
	
	GPIO_InitStructure.GPIO_Pin = RELAY1;		  		//设置继电器1对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//设置推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 		//工作速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  		//设置生效
	GPIO_ResetBits(GPIOA, RELAY1);				  		//默认低电平，继电器1不工作

    GPIO_InitStructure.GPIO_Pin = RELAY2;		  		//设置对应引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  		//设置生效
	GPIO_ResetBits(GPIOB, RELAY2);				  		//默认低电平，继电器2不工作
}

/**
 * 功能：翻转继电器状态
 * 参数：None
 * 返回值：None
 */
void toggleRelay(u16 xRelay)
{
    if(xRelay == RELAY1)
    {
        GPIOA->ODR ^= RELAY1;
    }else if(xRelay == RELAY2)
    {
        GPIOB->ODR ^= RELAY2;
    }else
    {

    }
}

/**
 * 功能：初始化Systick定时器
 * 参数：
 *         xRelayx:继电器标号 RELAY1,RELAY2
 *         sta:对应继电器状态 RELAY_OPEN,RELAY_CLOSE
 * 返回值：None
 */
void setRelay(u16 xRelay,RELAY_STATUS sta)
{
    if(xRelay == RELAY1)
    {
        if(sta == RELAY_OPEN)
        {
            GPIO_ResetBits(GPIOA, RELAY1);
        }else if(sta==RELAY_CLOSE)
        {
            GPIO_SetBits(GPIOA, RELAY1);
        }else 
        {

        }
        
    }else if(xRelay == RELAY2)
    {
        if(sta == RELAY_OPEN)
        {
            GPIO_ResetBits(GPIOB, RELAY2);
        }else if(sta==RELAY_CLOSE)
        {
            GPIO_SetBits(GPIOB, RELAY2);
        }else 
        {

        }
        
    }else
    {

    }    
}