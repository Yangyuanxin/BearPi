/******************************************************************
 * 文件：NEC.c
 * 功能：定义NEC编解码函数
 * 日期：2018-03-10
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "NEC/NEC.h"
#include "string.h"
#include "DELAY/Delay.h"

NEC_DataTypeDef NEC_DataStructure = {0,0,0,0,0,{0},0};

/**
 * 功能：发送NEC起始码
 * 参数：  None
 * 返回值：None
 */
static void codeStart(void)
{
    /*起始码*/
    HAVE_CARRIER();
    Delay_us(9000);
    NO_CARRIER();
    Delay_us(4500);
}

/**
 * 功能：发送一个字节
 * 参数：  
 *         data：待发送数据
 * 返回值：None
 */
static void codeByte(u8 data)
{
    u8 i;

    /*发送数据*/
    for(i=0;i<8;++i)
    {
        if((data>>i)&0x01)     //先发送低位
        {
            HAVE_CARRIER();
            Delay_us(560);
            NO_CARRIER();
            Delay_us(1690);
        }else 
        {
            HAVE_CARRIER();
            Delay_us(560);
            NO_CARRIER();
            Delay_us(560);
        }
    }
}

/**
 * 功能：发送一个字节
 * 参数：  
 *         repeatcnt：待发送重复码个数
 * 返回值：None
 */
static void codeRepeat(u8 repeatcnt)
{
	u8 i;
	
    if(repeatcnt==0)          //如果没有重复码就直接设置无载波，发射管进行空闲状态
    {
        HAVE_CARRIER();
        Delay_us(560);
        NO_CARRIER();     
    }else 
    {
        for(i=0;i<repeatcnt;++i)
        {
            HAVE_CARRIER();
            Delay_us(560);
            NO_CARRIER();
            Delay_ms(98);
            HAVE_CARRIER();
            Delay_us(9000);  
            NO_CARRIER();
            Delay_us(2250);                     
        }
        
		HAVE_CARRIER();
		Delay_us(560);
		NO_CARRIER(); 
    }
}

/**
 * 功能：初始化NEC
 * 参数：  None
 * 返回值：None
 */
void initNEC(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	/****************************红外接收IO初始化****************************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);           //开启GPIOB时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;                      //配置PB8 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //配置为浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

    /****************************红外发射IO初始化****************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;                      //配置PB7 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                 //配置为复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/**
 * 功能：NEC解码函数
 * 参数：
 *      pNECData:存放NEC相关数据，比如捕获时长的数组、码值和重复值个数
*       Addr    :遥控器地址码（用户码），用于校验数据
 * 返回值：None
 */
void decodeNEC(NEC_DataTypeDef* pNECData,u8 Addr)
{
    u8 i,j;
    u8* pdata = (u8*)pNECData;                    //将结构体指针强制转换为u8指针，方便存放码值
					
    for(i=0;i<4;++i)                              //循环读取4个码值       
    {
        for(j=0;j<8;++j)                          //读取单个码值
        {
            if(pNECData->NEC_Buffer[j+i*8+1] <270 && pNECData->NEC_Buffer[j+i*8+1] > 170)       //二进制1
            {
                *(pdata+i) |= 0x80>>(7-j);      
            }else if(pNECData->NEC_Buffer[j+i*8+1] <170 && pNECData->NEC_Buffer[j+i*8+1] > 70)  //二进制0
            {
                *(pdata+i) &= ~(0x80>>(7-j));
            }else                                                                               //无效值
            {
				memset(NEC_DataStructure.NEC_Buffer,0,35);
                return;
            }
			
			if(*pdata != Addr)                                                                  //防止数据错误
			{
				*pdata = 0;
				memset(NEC_DataStructure.NEC_Buffer,0,35);
				return ;
			}
        }
    }
	
	memset(NEC_DataStructure.NEC_Buffer,0,35);
	pNECData->RepeatCode = pNECData->NEC_Buffer[34];	                                       //重复码次数赋值
}

/**
 * 功能：NEC编码函数
 * 参数：
 *      addr:NEC地址码
 *      cmd:NEC命令码 
 *      repeatcnt:重复码个数
 * 返回值：None
 */
void codeNEC(u8 addr,u8 cmd,u8 repeatcnt)
{
    codeStart();                //发送起始码
 
    codeByte(addr);             //发送地址码
    codeByte(~addr);            //发送地址反码
    codeByte(cmd);              //发送命令码
    codeByte(~cmd);             //发送命令反码

    codeRepeat(repeatcnt);      //发送重复码
}