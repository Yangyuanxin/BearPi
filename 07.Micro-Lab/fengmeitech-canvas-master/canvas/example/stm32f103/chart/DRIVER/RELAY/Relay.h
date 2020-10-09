/******************************************************************
 * 文件：Relay.h
 * 功能：声明继电器函数
 * 日期：2018-03-11
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "stm32f10x.h"

typedef enum
{
    RELAY_OPEN =  0,  //常开触点断开，继电器断电
    RELAY_CLOSE = 1   //常开触点吸合，继电器工作
}RELAY_STATUS;

#define RELAY1 GPIO_Pin_7
#define RELAY2 GPIO_Pin_0

void initRelay(void);
void toggleRelay(u16 xRelay);
void setRelay(u16 xRelay,RELAY_STATUS sta);