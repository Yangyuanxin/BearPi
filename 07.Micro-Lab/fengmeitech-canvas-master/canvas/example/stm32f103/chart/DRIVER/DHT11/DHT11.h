/******************************************************************
 * 文件：DHT11.h
 * 功能：声明DHT11采集函数
 * 日期：2018-03-16
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"

/**************指定要获取的数据************/
typedef enum
{
    TEMPERATURE = 0,        //只获取温度
    HUMIDITY = 1,           //只获取湿度
    ALL = 2                 //温湿度同时获取
}DHT11_MODE;

/**************驱动IO宏定义*****************/
#define DATA_LINE_PORT GPIOB
#define DATA_LINE_PIN  GPIO_Pin_9
#define DATA_LINE_CLOCK RCC_APB2Periph_GPIOB

/**************数据线输出状态宏定义*****************/
#define LINE_HIGH()  GPIO_SetBits(DATA_LINE_PORT,DATA_LINE_PIN)
#define LINE_LOW()   GPIO_ResetBits(DATA_LINE_PORT,DATA_LINE_PIN) 


void initDHT11(void);
u16 readDHT11(void);
#endif