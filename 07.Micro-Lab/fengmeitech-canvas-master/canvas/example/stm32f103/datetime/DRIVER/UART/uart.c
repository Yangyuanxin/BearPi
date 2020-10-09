/******************************************************************
 * 文件：uart.h
 * 功能：实现uart相关函数
 * 日期：2018-02-27
 * 作者：zx
 * 版本：
 *       Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "UART/uart.h"
#include <stdio.h>
#include "NET/NetWork.h"

extern void processBytes(char * bytes, unsigned short len);
/**
 * 功能：初始化UART
 * 参数：None
 * 返回值：None
 */
void initUART(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);  //使能USART1时钟
    /*********************GPIO Config***************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                                      //发送管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                //复用推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                                     //接收管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                          //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /********************UART Config*****************************/ 
    USART_InitStructure.USART_BaudRate = 115200;                                     //设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8bits数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1bit停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                //发送接收均使能
	USART_Init(USART1, &USART_InitStructure);                                      //设置生效
	USART_Cmd(USART1, ENABLE);                                                     //使能串口外设

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	
}

/**
 * 功能：指定某个UART发送一个字节
 * 参数：USARTx：使用的目标串口x为1-3
 *       byte：待发送字节
 * 返回值：None
 */
void sendUartByte(USART_TypeDef *USARTx, u16 byte)
{
	USART_ClearFlag(USARTx, USART_FLAG_TC);             //软件清除发送完成标志位
	USART_SendData(USARTx, byte);                       //发送一个字节
	while (!USART_GetFlagStatus(USARTx, USART_FLAG_TC));//等待发送完成
	USART_ClearFlag(USARTx, USART_FLAG_TC);             //软件清除发送完成标志位
}

/**
 * 功能：使用UART1 发送多个字节
 * 参数：USARTx：使用的目标串口x为1-3
 *       byte：待发送字节
 * 返回值：None
 */
void sendUartBytes(char * bytes, u16 len)
{
	for(unsigned short i=0;i<len;++i)
	{
		sendUartByte(USART1, bytes[i]);
	}
}


/****************************中断服务函数****************************************/
/**
 * 功能：串口1中断服务函数
 * 参数：None
 * 返回值：None
 */
void USART1_IRQHandler(void)
{
	static u8 tail = 0;
	char rbuffer[100];
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE)) //判断接收数据寄存器是否有数据
	{
		rbuffer[tail++] = USART_ReceiveData(USART1);
		
		if(tail>=100)
		{
			tail = 0;
		}
	}

	if(USART_GetITStatus(USART1, USART_IT_IDLE))
	{
		USART_ReceiveData(USART1);              //读一次UART可以清除空闲标志位
		processBytes(rbuffer, tail);
		tail = 0;
	}   
}

/**
 * 功能：串口2中断服务函数
 * 参数：None
 * 返回值：None
 */
void USART2_IRQHandler(void)
{

}

/**
 * 功能：串口3中断服务函数
 * 参数：None
 * 返回值：None
 */
void USART3_IRQHandler(void)
{
}

/****************************重定向fputc函数****************************************/
/**
 * 功能：使用UART1实现printf函数功能
 * 参数：ch:待写入字符 我们无需关心
 *       f：文件指针 我们无需关心
 * 返回值：None
 */
int fputc(int ch, FILE *f)
{
    /* 将Printf内容发往串口 */
    USART_SendData(USART1, (unsigned char)ch);
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TC));
    USART_ClearFlag(USART1, USART_FLAG_TC);

    return (ch);
}
