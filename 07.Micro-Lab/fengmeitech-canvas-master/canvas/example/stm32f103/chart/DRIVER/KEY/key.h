#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"
#include "DELAY/Delay.h"

typedef enum 
{
    KEY_NO = 0x00,   //没有按键按下
    KEY_UP = 0x01,   //UP键按下
    KEY_DOWN = 0x02, //DOWN键按下
    KEY_ALL = 0x03   //同时按下
}KEY_VALUE;

typedef enum 
{
    KEY_PRESS = 0x01,  //按下按键
    KEY_RELEASE = 0x02 //释放按键
}KEY_MODE;

#define KEY_UP_PIN    GPIO_Pin_0 //宏定义KEYUP引脚
#define KEY_DOWN_PIN  GPIO_Pin_1 //宏定义KEYDWON引脚

void initKey(void);
u8 getKeyValue(u8 mode);

#endif
