/******************************************************************
 * 文件：Buzzer.c
 * 功能：实现蜂鸣器相关函数
 * 日期：2018-03-12
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "BUZZER/Buzzer.h"
#include "TIMx/TIMx.h"


/**
 * 功能：初始化蜂鸣器对应IO，该函数包括了重映射部分
 *       需要注意，
 * 参数：
 *       vol : 默认音量
 *       tone：默认音调
 * 返回值：None
 */
void initBuzzer(u8 vol,u16 tone)
{
    GPIO_InitTypeDef GPIO_InitStructure;                                        //定义GPIO初始化结构体变量

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //使能GPIOB和复用功能时钟

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                        //将TIM3部分重映射到PB4

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                             //设置为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                                    //设置引脚4
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                           //工作速度50MHz    
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                      //初始化设置生效   

    setBuzzerVol(vol);                                                          //设置蜂鸣器默认音量
    setBuzzerTone(tone);                                                        //设置蜂鸣器默认音调
}

/**
 * 功能：设置蜂鸣器的音量
 * 参数：
 *          vol:音量大小 0-99 0就是静音 99就是最大音量
 * 返回值：None
 */
void setBuzzerVol(u8 vol)
{
    TIM_SetCompare1(TIM3,getPeriod(TIM3)*vol/100);
}

/**
 * 功能：设置蜂鸣器的音调
 * 参数：
 *          tone:音调大小单位Hz，建议100-10000
 *               另外我们封装了七音阶音调，方便大家使用
 * 注意：   这里是以定时器时钟频率10us进行计算的
 * 返回值：None
 */
void setBuzzerTone(u16 tone)
{
    setPeriod(TIM3,100000/tone);
}
