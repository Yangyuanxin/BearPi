/******************************************************************
 * 文件：uart.h
 * 功能：声明uart相关函数
 * 日期：2018-02-27
 * 作者：zx
 * 版本：
 *       Ver.1.0 | 最初版本
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __UART_H
#define __UART_H
#include "stm32f10x.h"

void initUART(void);
void sendUartByte(USART_TypeDef* USARTx,u16 byte);
void sendUartBytes(char * bytes, u16 len);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);

#endif
