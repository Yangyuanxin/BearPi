 /***********************************************************************************
  * file   : canvas.c
  * brief  : update MicroLab`s canvas & get canvas event
  * author : zx
  * Data   : 2020/4/28-21:58
  * Ver    :
  *             -Ver.1.0 first add 
  * Note   : Developers don't need to care about implementation details, just learn how to use these two function "updateCanvas", "getEvent"
  * BBS    : http://www.openelec.cn/
  * 
  * Copyright (C) 2020-2021 zx. All rights reserved.
  ***********************************************************************************/ 
#include "canvas.h"
#include "protocol.h"
#include <string.h>
//注释
//#include "UART/uart.h"
#include "usart.h"

CanvasEventList events;

void uart1_send_byte(uint8_t one_byte)
{
    uint32_t ulReturn;
    HAL_UART_Transmit(&huart1, &one_byte, 1, 1000);
    while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) != SET);
}

/**
 * 功能：使用UART1 发送多个字节
 * 参数：USARTx：使用的目标串口x为1-3
 *       byte：待发送字节
 * 返回值：None
 */
void sendUartBytes(char * bytes, uint16_t len)
{
	for(unsigned short i=0;i<len;++i)
	{
		uart1_send_byte(bytes[i]);
	}
}

/**
 * brief : sendbytes
 * parameter:
 *      bytes        : bytes that need send
 *      len          : lenth of bytes
 * ret: none
 */
static void sendBytes(char * bytes, unsigned short len)
{
    /*you need input a function that send bytes with uart&TCP/IP*/
		sendUartBytes(bytes, len);
}

/**
 * brief    : process receiveed bytes that from uart&TCP/IP, this function is called in ISR
 * parameter:
 *      bytes        : bytes that need send
 *      len          : lenth of bytes
 * ret      : none
 * 
 */
void processBytes(char * bytes, unsigned short len)
{
    unsigned char index = 0;
	
    ProtocolObject protocol;
	char msg[MSGSIZE];
	unsigned short msglen;
	
    if(unpackProtocol(&protocol, bytes, len, msg, &msglen)==0)
    {
		for(unsigned char i=0;i<CANVASEVENTLIST_SIZE;++i)
		{
			if(events[i].componenttype==protocol.componenttype)
			{
				events[i].componenttype = protocol.componenttype;
				events[i].componentnumer = protocol.componentnum;
				events[i].componentmsgtype = protocol.msgtype;	
				events[i].msglen = msglen;
				memcpy(events[i].msg, msg, msglen);
				
				break;
			}else 
			{
				if(events[i].componenttype==NONE_COMPONENT)
				{
					index = i;
				}
			}
			
			if(i==CANVASEVENTLIST_SIZE-1)
			{
				events[index].componenttype = protocol.componenttype;
				events[index].componentnumer = protocol.componentnum;
				events[index].componentmsgtype = protocol.msgtype;	
				events[index].msglen = msglen;
				memcpy(events[index].msg, msg, msglen);
			}
		}

    }else
    {
        return ;
    }
}

/**
 * brief : init Canvas
 * parameter:
 *      events        : bytes that need send
 *      len          : lenth of bytes
 * ret: none
 */
void initCanvas(CanvasEventList events)
{
	for(unsigned char i=0;i<CANVASEVENTLIST_SIZE;++i)
	{
		events[i].componenttype = NONE_COMPONENT;
		events[i].componentnumer = 0;
		events[i].componentmsgtype = NONE_MSG;
	}
}

/**
 * brief : update Canvas`components state with serialport/net
 * parameter:
 *      componenttype  : component type
 *      componentnumber: component number
 *      data           : data that to canvas
 *      datalen        : data lenth
 * ret: none
 */
void updateCanvas(COMPONENT_TYPE componenttype, unsigned short componentnumber, char * data, unsigned short datalen)
{
    static char tbuffer[TBUFFERSIZE];
    static short size;

    packProtocol(ORGANIZATION, SECTION, DATAPOINT, componenttype, componentnumber, NONE_MSG, data, datalen, tbuffer, &size);
    sendBytes(tbuffer, size);
}
