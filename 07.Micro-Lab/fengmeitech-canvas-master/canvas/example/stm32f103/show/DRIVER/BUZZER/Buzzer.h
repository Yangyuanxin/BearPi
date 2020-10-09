/******************************************************************
 * 文件：Buzzer.h
 * 功能：声明蜂鸣器相关函数
 * 日期：2018-03-12
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __BUZZER_H
#define __BUZZER_H
#include "stm32f10x.h"

/********音调，和ToneArray数组配合使用*************/
typedef enum
{
    DO = 0,
    RE = 1,
    MI = 2,
    FA = 3,
    SO = 4,
    LA = 5,
    SI = 6
}BUZZER_TONE;

/********音量*************/
typedef enum
{
    MAXVOL = 99,  //最大音量
    MUTE   = 0,   //静音
}BUZEER_VOL;

void initBuzzer(u8 vol,u16 tone);
void setBuzzerVol(u8 vol);
void setBuzzerTone(u16 tone);

#endif
