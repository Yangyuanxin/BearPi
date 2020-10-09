/******************************************************************
 * 文件：DHT11.c
 * 功能：实现DHT11采集函数
 * 日期：2018-03-16
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "DHT11/DHT11.h"
#include "DELAY/Delay.h"

/**
 * 功能：设置数据线为输入
 * 参数：None
 * 返回值：None
 */
static void setLineIn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = DATA_LINE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    
    GPIO_Init(DATA_LINE_PORT,&GPIO_InitStructure);
}

/**
 * 功能：设置数据线为输出
 * 参数：None
 * 返回值：None
 */
static void setLineOut(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DATA_LINE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(DATA_LINE_PORT,&GPIO_InitStructure);
    GPIO_SetBits(DATA_LINE_PORT,DATA_LINE_PIN);
}

/**
 * 功能：读取单个字节,整个读取流程需要调用该函数4次
 * 参数：None
 * 返回值：采集到的字节数据
 */
static u8 readByte(void)
{
    u8 i,byte = 0;
	u8 j = 0;

    for(i=0;i<8;++i)
    { 
        while(!GPIO_ReadInputDataBit(DATA_LINE_PORT,DATA_LINE_PIN) && ++j<10)      //等待低电平结束
		{
			Delay_us(10);
		}
		j = 0;
		
        Delay_us(40);                                                             //延时 用于后续判断
        if(GPIO_ReadInputDataBit(DATA_LINE_PORT,DATA_LINE_PIN))                   //延时后为高电平则为二进制1
        { 
            byte <<= 1;
            byte |= 0x01;
        }else 
        { 
            byte <<= 1;
        }

        while(GPIO_ReadInputDataBit(DATA_LINE_PORT,DATA_LINE_PIN) && ++j<10)      //等待剩余高电平
		{
			Delay_us(10);
		}
    }
    return byte;    
}

/**
 * 功能：初始化DHT11 GPIO
 * 参数：None
 * 返回值：采集到的字节数据
 */
void initDHT11(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(DATA_LINE_CLOCK, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = DATA_LINE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DATA_LINE_PORT,&GPIO_InitStructure);
	
    GPIO_SetBits(DATA_LINE_PORT,DATA_LINE_PIN);

    Delay_ms(1500);                                     //开机稳定1.5S
}

/**
 * 功能：读取DHT11采集到的数据
 * 参数：
 *        mode：返回数据类型 用户可以指定返回温湿度当中的某一个或者所有
 *              返回所有数据时，湿度在高八位，温度在低八位
 * 返回值：采集到的字节数据
 *        
 */
u16 readDHT11(void)
{
    u8 i = 0;
    
    u8 Hum_H,Hum_L,Temp_H,Temp_L,CheckByte;                                 //依次是湿度整数、小数部分、温度整数、小数部分、校验和字节
    
    setLineOut();                                                           //发起读取开始信号
    LINE_LOW();
    Delay_ms(25);
    LINE_HIGH();

    setLineIn();

    while(GPIO_ReadInputDataBit(DATA_LINE_PORT,DATA_LINE_PIN) && ++i<10)   //主机释放总线空闲阶段
    {
        Delay_us(10);
    }
    i = 0;
    while(!GPIO_ReadInputDataBit(DATA_LINE_PORT,DATA_LINE_PIN) && ++i<10)  //传感器响应阶段
    {
        Delay_us(10);
    }
    i = 0;
    while(GPIO_ReadInputDataBit(DATA_LINE_PORT,DATA_LINE_PIN) && ++i<10)   //传感器拉高通知主机准备阶段
    {
        Delay_us(10);
    }

    Hum_H = readByte();                                                    //读取有效湿度值
    Hum_L = readByte();                                                    //湿度小数部分为0
    Temp_H = readByte();                                                   //读取有效温度值 
    Temp_L = readByte();                                                   //温度小数部分为0
    CheckByte = readByte();                                                //校验和                         

    if(Hum_H+Hum_L+Temp_H+Temp_L==CheckByte)                               //计算校验和
    {
        return Hum_H<<8 | Temp_H;

    }else 
    {		
        return 0xFFFF;                                                     //返回无效值，标识采集有误                                                
    }
        
}
