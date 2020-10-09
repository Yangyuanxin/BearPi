/******************************************************************
 * 文件：NEC.h
 * 功能：声明NEC编解码函数
 * 日期：2018-03-10
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __NEC_H
#define __NEC_H

#include "stm32f10x.h"

/**********************是否产生载波宏定义**********************/
#define HAVE_CARRIER()  TIM_SetCompare2(TIM4,9)
#define NO_CARRIER()    TIM_SetCompare2(TIM4,0)

/*********************NEC数据码结构体*************************/
typedef struct
{
    u8 AddrCode_H;      //地址码
    u8 AddrCode_L;      //地址反码
    u8 CmdCode_H;       //命令码
    u8 CmdCode_L;       //命令反码
        
    u16 RepeatCode;     //重复码个数，该变量成员记录的是重复码的个数
    /**
     * 存放输入捕获采集到的NEC码对应时长，单位为10us
     * 1个起始码+32个二进制位码占用数组NEC_Buffer[0]-NEC_Buffer[32]
     * 重复码次数存放于数组最后一个元素中
     */
    u16 NEC_Buffer[35]; //存放捕获到的时间长度
	
	u8 DecodeOver;      //暂时没有用到，预留
}NEC_DataTypeDef;

void initNEC(void);
void decodeNEC(NEC_DataTypeDef* pNECData, u8 Addr);
void codeNEC(u8 addr,u8 cmd,u8 repeatcnt);

#endif