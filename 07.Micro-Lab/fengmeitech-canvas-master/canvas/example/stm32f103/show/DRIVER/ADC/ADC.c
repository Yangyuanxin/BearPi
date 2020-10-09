/******************************************************************
 * 文件：ADC.c
 * 功能：实现ADC驱动功能函数
 * 日期：2018-03-1
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "ADC/ADC.h"
#include "DELAY/Delay.h"

/**
 * 功能：初始化ADC1_IN9
 * 参数：None
 * 返回值：None
 */
void initADC(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                                        //定义GPIO初始化结构体
    ADC_InitTypeDef ADC_InitStructure;                                          //定义ADC初始化结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE); //使能外设时钟
	
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 					//重映射JTAG引脚,只使用SWD模式
   
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);                        					//设置ADC时钟分频，此时ADC时钟频率为72/8=9MHz

    /*********************配置PB1***************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;            					//模拟输入引脚
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/**************************************************************************
	 * 开发板上PA15上电默认是JTAG的JTDI引脚，为上拉输出，这会导致冷光灯亮起，影响ADC采集效果
	 * 因此要将PA15重映射到该引脚上
	 **************************************************************************/
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;									//设置对应引脚
	// GPIO_Init(GPIOA, &GPIO_InitStructure);			 	   					 	//设置生效
	// GPIO_ResetBits(GPIOA, GPIO_Pin_15);				  							//默认低电平

    /*********************配置ADC***************************/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                          //配置为独立工作模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                               //设置ADC工作在单通道
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                         //设置ADC为单次转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;         //失能外部触发，使用软件人为触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      //设置ADC数据寄存器右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                                     //规则转换通道数，我们只使用一个
    ADC_Init(ADC1, &ADC_InitStructure);                                         //生效ADC设置

    ADC_Cmd(ADC1, ENABLE);                                                      //设置ADC_CR2中的ADON位，使其给ADC1上电

    /*********************************************************************
     * 1. 配置ADCx规则转换的通道以及转换序列
     * 2. 配置转换序列，因为我们只用到了一个通道，因此转换序列要设置为1
     * 3. 设置ADCx的采样周期
     *********************************************************************/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5);

    ADC_ResetCalibration(ADC1);                                                 //复位ADC1校准
    while (ADC_GetResetCalibrationStatus(ADC1));                                //等待复位校准完成
    ADC_StartCalibration(ADC1);                                                 //开始ADC1的校准
    while (ADC_GetCalibrationStatus(ADC1));                                     //等待校准完成
}

/**
 * 功能：获取ADC1_IN9单次AD转换结果
 * 参数：None
 * 返回值：None
 */
u16 getConvValue(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);         //开启软件转换，置位ADC_CR2的SWSTART位
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //等待转换完成
    return ADC_GetConversionValue(ADC1);            //获取ADC转换结果
}

/**
 * 功能：获取ADC1_IN9多次转换平均值
 * 参数：counts：采集次数
 *       xus：每次采集的时间间隔，单位为us，建议时间不要太长
 * 返回值：多次平均采集结果
 */
u16 getConvValueAve(u8 counts, u32 xus)
{
    u8 i;
    u32 sum = 0;               //累加值临时存放变量

    for (i = 0; i < counts; ++i)
    {
        sum += getConvValue(); //采样结果累加
        Delay_us(xus);
    }

    return sum / counts;       //取平均值并返回
}
