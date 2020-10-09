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
					case PUSHBUTTON:
					{
						if(events[i].componentnumer==0)
						{
							if(events[i].componentmsgtype==PUSHBUTTON_PRESS)
							{
								printf("PUSHBUTTON 0 PRESSED, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else if(events[i].componentmsgtype==PUSHBUTTON_RELEASE)
							{
								printf("PUSHBUTTON 0 RELEASED, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else
							{
								
							}
						}else if(events[i].componentnumer==1)
						{
							if(events[i].componentmsgtype==PUSHBUTTON_PRESS)
							{
								printf("PUSHBUTTON 1 PRESSED, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else if(events[i].componentmsgtype==PUSHBUTTON_RELEASE)
							{
								printf("PUSHBUTTON 1 RELEASED, ");
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
					case SWITCHBUTTON:
					{
						if(events[i].componentnumer==0)
						{
							if(events[i].componentmsgtype==SWITCH_ON)
							{
								printf("SWITCHBUTTON 0 ON, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else if(events[i].componentmsgtype==SWITCH_OFF)
							{
								printf("SWITCHBUTTON 0 OFF, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else
							{
								
							}
						}else if(events[i].componentnumer==1)
						{
							if(events[i].componentmsgtype==SWITCH_ON)
							{
								openLED();
								printf("SWITCHBUTTON 1 ON, ");
								printf("MSG : ");
								for(unsigned short j=0;j<events[i].msglen;++j)
								{
									printf("%2X ", events[i].msg[j]);								
								}
								printf("\r\n");
							}else if(events[i].componentmsgtype==SWITCH_OFF)
							{
								closeLED();
								printf("SWITCHBUTTON 1 OFF, ");
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
					case SLIDER:
					{
						if(events[i].componentnumer==0)
						{
							printf("SLIDER 0, VALUE:%d", *(unsigned int *)events[i].msg);
							printf("\r\n");
						}else if(events[i].componentnumer==1)
						{
							printf("SLIDER 1, VALUE:%d", *(unsigned int *)events[i].msg);
							printf("\r\n");						
						}							
					}							break;
					case DIAL:
					{
						if(events[i].componentnumer==0)
						{
							printf("DIAL 0, VALUE:%d", *(unsigned int *)events[i].msg);
							printf("\r\n");
						}else if(events[i].componentnumer==1)
						{
							printf("DIAL 1, VALUE:%d", *(unsigned int *)events[i].msg);
							printf("\r\n");						
						}			
					}							break;
					case KEYBOARD:				
					{
						if(events[i].componentmsgtype==KEY_PRESS)
						{
							printf("KEYBOARD, VALUE:%d", *(unsigned int *)events[i].msg);
							printf("\r\n");						
						}else
						{
							
						}
					
					}							break;
					case MOUSE:
					{
						static int wheel = 0;
						if(events[i].componentmsgtype==MOUSE_PRESS)
						{
							printf("MOUSE PRESS, VALUE:%d", *(unsigned int *)events[i].msg);
							printf("\r\n");								
						}else if(events[i].componentmsgtype==MOUSE_WHEEL)
						{
							wheel += *(unsigned int *)events[i].msg;
							printf("MOUSE WHEEL, VALUE:%d", wheel);
							printf("\r\n");									
						}else
						{
							
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

