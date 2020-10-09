/******************************************************************
 * 文件：main.c
 * 功能：主函数入口
 * 日期：2018-02-16
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 官博：http://fengmeitech.club
 * Copyright (C) 2017 zx. All rights reserved.
*******************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "DELAY/Delay.h"
#include "DHT11/DHT11.h"
#include "LED/LED.h"
#include "UART/uart.h"
#include "canvas.h"

extern CanvasEventList events;
int main(void)
{
	unsigned char cnts = 0;
	
	u16 dht11 = 0;

	/*初始化各外设*/ 
	initSysTick(); 
	initNVIC(NVIC_PriorityGroup_2);
	initUART();
	initLED();
	initCanvas(events);
	
	while (1)
	{
		for(char i=0;i<CANVASEVENTLIST_SIZE;++i)
		{
			if(events[i].componenttype!=NONE_COMPONENT)
			{
				switch(events[i].componenttype)
				{
					case CLOCK:
					{
						if(events[i].componentnumer==0)
						{
							if(events[i].componentmsgtype==CLOCK_PERIOD)
							{
								printf("CLOCK 0 PERIOD, ");
								printf("DATETIME : %d-%d-%d  %d:%d:%d\r\n", *(short *)events[i].msg, events[i].msg[2], events[i].msg[3], events[i].msg[4], events[i].msg[5], events[i].msg[6]);
							}else if(events[i].componentmsgtype==CLOCK_ALARM)
							{
								printf("CLOCK 0 ALARM, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else
							{
								
							}
						}else
						{
							/*other number*/
						}
					}							break;
					case LCDDATETIME:
					{
						if(events[i].componentnumer==0)
						{
							if(events[i].componentmsgtype==DATETIME_PERIOD)
							{
								printf("LCDDATETIME 0 PERIOD, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else if(events[i].componentmsgtype==DATETIME_ALARM)
							{
								printf("LCDDATETIME 0 ALARM, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else
							{
								
							}
						}else
						{
							/*other number*/
						}
						
					}							break;
					
					default:					break;
				}
		
				events[i].componenttype = NONE_COMPONENT;
				events[i].componentnumer = 0;
				events[i].componentmsgtype = NONE_MSG;
				events[i].msglen = 0;	
			}else
			{
				/*nothing..*/
			}
		}	
	}
}

