/******************************************************************
 * 文件：key.c
 * 功能：实现LED驱动功能函数
 * 日期：2018-02-23
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "KEY/key.h"

/**
 * 功能：初始化按键
 * 参数：None
 * 返回值：None
 */
void initKey(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义GPIO初始化结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIO时钟

    GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN|KEY_DOWN_PIN;		  //设置按键对应引脚 UP->PA0, DOWN->PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //设置上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  //设置生效
}

/**
 * 功能：获取按键值，支持同时按下，按键释放后生效
 * 参数：mode:用于指定按键触发方式，参数可选：KEY_PRESS,KEY_RELEASE
 * 返回值：返回按键值，当同时按下时，返回值是 KEY_UP|KEY_DOWN 即0x03
 */
u8 getKeyValue(u8 mode)
{
    u8 keyval = 0;

    if(mode == KEY_PRESS) //按下按键即可生效
    {
         /**************检测UP键函数段********************/
        if(GPIO_ReadInputDataBit(GPIOA,KEY_UP_PIN)==0) //第一次检测时按下
        {
            Delay_ms(10); //延时去抖
            if(GPIO_ReadInputDataBit(GPIOA,KEY_UP_PIN)==0) //第二次检测还为零则可以确定按键按下
            {
                keyval |= KEY_UP;
            }else 
            {
                keyval &= ~KEY_UP;
            }
        }else 
        {
            keyval &= ~KEY_UP;  //没有按下 对应位清零
        }

        /**************检测DOWN键函数段********************/
        if(GPIO_ReadInputDataBit(GPIOA,KEY_DOWN_PIN)==0) //第一次检测时按下
        {
            Delay_ms(10); //延时去抖
            if(GPIO_ReadInputDataBit(GPIOA,KEY_DOWN_PIN)==0) //第二次检测还为零则可以确定按键按下
            {
                keyval |= KEY_DOWN;
            }else 
            {
                keyval &= ~KEY_DOWN;
            }
        }else 
        {
            keyval &= ~KEY_DOWN;  //没有按下 对应位清零
        }
    }else if(mode == KEY_RELEASE) //松开按键生效
    {
         /**************检测UP键函数段********************/
        if(GPIO_ReadInputDataBit(GPIOA,KEY_UP_PIN)==0) //第一次检测时按下
        {
            Delay_ms(10); //延时去抖
            if(GPIO_ReadInputDataBit(GPIOA,KEY_UP_PIN)==0) //第二次检测还为零则可以确定按键按下
            {
                keyval |= KEY_UP;
            }else 
            {
                keyval &= ~KEY_UP;
            }
        }else 
        {
            keyval &= ~KEY_UP;  //没有按下 对应位清零
        }

        /**************检测DOWN键函数段********************/
        if(GPIO_ReadInputDataBit(GPIOA,KEY_DOWN_PIN)==0) //第一次检测时按下
        {
            Delay_ms(10); //延时去抖
            if(GPIO_ReadInputDataBit(GPIOA,KEY_DOWN_PIN)==0) //第二次检测还为零则可以确定按键按下
            {
                keyval |= KEY_DOWN;
            }else 
            {
                keyval &= ~KEY_DOWN;
            }
        }else 
        {
            keyval &= ~KEY_DOWN;  //没有按下 对应位清零
        } 

        while(GPIO_ReadInputDataBit(GPIOA,KEY_UP_PIN)==0 || GPIO_ReadInputDataBit(GPIOA,KEY_DOWN_PIN)==0); //等待按键释放
    }

    return keyval;
}