/******************************************************************
 * 文件：IIC.c
 * 功能：实现模拟IIC基本函数
 * 日期：2018-03-04
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "IIC/IIC.h"
#include "DELAY/Delay.h"

/**
 * 功能：设置SDA引脚为输入状态，用于检测从机的ACK信号以及接收数据
 * 参数：None
 * 返回值：None
 */
void setSDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                  //定义GPIO初始化结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIO时钟

    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;		      //SDA引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //设置上拉输入

	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);	      //设置生效
}

/**
 * 功能：设置SDA引脚为输出状态，用于向外发送数据
 * 参数：None
 * 返回值：None
 */
void setSDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                  //定义GPIO初始化结构体

    RCC_APB2PeriphClockCmd(IIC_SDA_RCC, ENABLE);          //使能GPIO时钟

    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;            //SDA引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;      //开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);         //设置生效    
    GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN);               //SDA拉高 
}

/**
 * 功能：初始化模拟IIC引脚
 * 参数：None
 * 返回值：None
 */
void initIIC(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;             //定义GPIO初始化结构体

    RCC_APB2PeriphClockCmd(IIC_SDA_RCC, ENABLE);     //使能SDA时钟

    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;       //设置SDA引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置输出速度
    GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);    //设置生效    
    GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN);          //拉高SDA  

    RCC_APB2PeriphClockCmd(IIC_SCL_RCC, ENABLE);     //使能SCL时钟
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;       //设置SCL引脚
    GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);    //设置生效    
    GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN);          //拉高SCL   
}

/**
 * 功能：发起IIC开始信号
 * 参数：None
 * 返回值：None
 */
void startIIC(void)
{
    IIC_SDA_OUT();          //设置SDA引脚为开漏输出

    IIC_SCL_H();            //拉高SCL
    IIC_SDA_H();            //拉高SDA
    Delay_us(IIC_SPEED);    //延时一段时间
    IIC_SDA_L();            //拉低SDA，在SCK高电平器件产生下降沿
    Delay_us(IIC_SPEED);    //延时一段时间
    IIC_SCL_L();            //拉低时钟，完成一个时钟周期
}

/**
 * 功能：发起IIC停止信号
 * 参数：None
 * 返回值：None
 */
void stopIIC(void)
{
    IIC_SDA_OUT();

    IIC_SDA_L();
    IIC_SCL_H();
    Delay_us(IIC_SPEED);
    IIC_SDA_H();            //SCL高电平期间，产生SDA下降沿
    Delay_us(IIC_SPEED);
    IIC_SCL_L();
}

/**
 * 功能：发起一个字节数据
 * 参数：byte：待发送数据
 * 返回值：None
 */
void sendIICByte(u8 byte)
{
    u8 i;

    IIC_SDA_OUT();

    for(i=0;i<8;++i)
    {
        if(byte & 0x80)
        {
            IIC_SDA_H();
        }else 
        {
            IIC_SDA_L();
        }
        byte <<= 1;

        IIC_SCL_H();
        Delay_us(IIC_SPEED);
        IIC_SCL_L();
        Delay_us(IIC_SPEED);
    }
}

/**
 * 功能：接收一个字节数据
 * 参数：None
 * 返回值：返回采集到的数据
 */
u8 receiveIICByte(void)
{
    s8 i;
    u8 byte = 0;

    IIC_SDA_IN();
    Delay_us(IIC_SPEED);
    for(i=7;i>=0;--i)
    {
        IIC_SCL_H();
        Delay_us(IIC_SPEED);
        if(GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN))
        {
            byte |= 0x01<<i;
        }else 
        {
            byte |= 0x00<<i;
        }

        IIC_SCL_L();
        Delay_us(IIC_SPEED);
    }

    return byte;
}

/**
 * 功能：发送响应信号
 * 参数：None
 * 返回值：None
 */
void sendIICAck(void)
{
    IIC_SDA_OUT();

    IIC_SDA_L();
    IIC_SCL_H();
    Delay_us(IIC_SPEED);
    IIC_SDA_H();
    IIC_SCL_L();
}

/**
 * 功能：发送非响应信号
 * 参数：None
 * 返回值：None
 */
void sendIICNAck(void)
{
    IIC_SDA_OUT();

    IIC_SDA_H();
    IIC_SCL_H();
    Delay_us(IIC_SPEED);
    IIC_SCL_L();
}
/**
 * 功能：等待从机ACK信号
 * 参数：None
 * 返回值：None
 */
IIC_ACK waitAck(void)
{
    u8 i = 0;

    IIC_SDA_IN();
    IIC_SCL_H();
    while(GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN))
    {
        if(++i>50)
        {
            IIC_SCL_L();
            return  NACK;
        }
        Delay_us(1);
    }

    IIC_SCL_L();
    return ACK;
}

