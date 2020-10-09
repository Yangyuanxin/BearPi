/******************************************************************
 * 文件：DMA.c
 * 功能：实现DMA相关函数
 * 日期：2018-03-15
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "DMA/DMA.h"

/**
 * 功能：初始化DMA
 * 参数：
 *          DMA_Chx:设置使用的DMA通道 DMA1_Channel1-DMA1_Channel7
 *          periph_addr:外设地址
 *          memory_addr：存储器地址
 * 返回值：None
 */
void initDMA(DMA_Channel_TypeDef* DMA_Chx, u32 periph_addr, u32 memory_addr, u16 trans_size)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA_Chx);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = periph_addr;                       //外设地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = memory_addr;                           //存储器地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                            //数据方向为存储器到外设
	DMA_InitStructure.DMA_BufferSize = trans_size;                                //单次发送数据个数
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              //外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       //内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   //设置外设数据位宽为半字(16位)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;       //设置存储器数据位宽为半字(16位)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 //正常在工作模式，另外一个是循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //设置最高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                  //失能存储器到存储器传输
	DMA_Init(DMA_Chx, &DMA_InitStructure);                                        //初始化配置生效

    DMA_Cmd(DMA_Chx, ENABLE);
}