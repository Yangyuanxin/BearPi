/******************************************************************
 * 文件：Light.c
 * 功能：定义Light相关函数
 * 日期：2018-03-14
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "LIGHT/Light.h"
#include "DELAY/Delay.h"

u16 RGB_buffer[69] = {0};
/**
 * 功能：初始灯光
 * 参数：
 *          brightness:初始化亮度 10-100
 *          colortemp:初始化色温 0-100     
 *          red:RGB红色比例 0-255
 *          green:RGB绿色比例 0-255
 *          blue：RGB蓝色比例 0-255    
 * 返回值：None
 */
void initLight(u8 brightness,u8 colortemp,u8 red,u8 green,u8 blue)
{
	u8 i;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //禁止JTAG保留SWD
//    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE); 	    //设置JTAG为定时器2部分映射,只使用SWD模式

    /*设置冷光灯*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);

    /*设置暖光灯*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    GPIO_ResetBits(GPIOB,GPIO_Pin_3);

    /*设置RGB*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    setRGB(red,green,blue);
}

/**
 * 功能：设置灯光亮度和色温
 * 参数：
 *          brightness:亮度 10-100
 *          colortemp:色温 0-100         
 * 返回值：None
 */
void setLight(u8 brightness,u8 colortemp)
{
    /**
     * 无论是亮度还是色温，最终都体现在LED的亮度上
     * LED最终的亮度计算公式为：满占空比(重装载值) * 亮度百分比 * 色温百分比
     * 并且要保证冷光和暖光的色温比值之和为100%
     * */
    TIM_SetCompare1(TIM2,getPeriod(TIM2)*brightness/100*colortemp/100);         //设置冷光 对应PA15 TIM2_CH1
    TIM_SetCompare2(TIM2,getPeriod(TIM2)*brightness/100*(100-colortemp)/100);   //设置暖光 对应PB3  TIM2_CH2
}


/**
 * 功能：设置RGB颜色
 * 参数：   
 *          red:RGB红色比例 0-255
 *          green:RGB绿色比例 0-255
 *          blue：RGB蓝色比例 0-255    
 * 返回值：None
 */
void setRGB(u8 red,u8 green,u8 blue)
{
    u8 i = 0;
	
    u32 rgb_value = green<<16 | red<<8 | blue;
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
	
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1,sizeof(RGB_buffer));
    for(i=0;i<24;++i)
    {
        if((rgb_value<<i)&0x800000)     //高位先发，此时高位为1时
        {
            RGB_buffer[i+45] = 61;       //68%占空比           
        }else 
        {
            RGB_buffer[i+45] = 28;       //32%占空比
        }
    }
	DMA_Cmd(DMA1_Channel1, ENABLE);
}