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
#include <string.h>

extern CanvasEventList events;

int main(void)
{
	float x = 0, y = 0, temp;
	char str[20];
	
	/*初始化各外设*/ 
	initSysTick(); 
	initNVIC(NVIC_PriorityGroup_2);
	initUART();
	initLED();
	initCanvas(events);
	
	while (1)
	{
		/*水位进度组件*/
		updateCanvas(WATERLEVER, 0, (char *)&x, sizeof(float));
		Delay_ms(100);	
		updateCanvas(WATERLEVER, 1, (char *)&x, sizeof(float));
		Delay_ms(100);	
		
		/*汽车仪表盘组件*/
		updateCanvas(CARDASHBOARD, 0, (char *)&x, sizeof(float));
		Delay_ms(100);	
		updateCanvas(CARDASHBOARD, 1, (char *)&x, sizeof(float));
		Delay_ms(100);	
		
		/*电池电量组件*/
		updateCanvas(BATTERY, 0, (char *)&x, sizeof(float));
		Delay_ms(100);	
		updateCanvas(BATTERY, 1, (char *)&x, sizeof(float));
		Delay_ms(100);
		
		/*指南针组件*/
		updateCanvas(COMPASS, 0, (char *)&x, sizeof(float));
		Delay_ms(100);	
		
		/*数码管组件*/
		updateCanvas(LCDNUMBER, 0, (char *)&x, sizeof(float));
		Delay_ms(100);	
		
		/*LED点阵组件*/
		temp = (int)x/10;
		updateCanvas(LEDDOT, 0, (char *)&temp, sizeof(float));
		Delay_ms(100);	
		temp = (int)x%10;
		updateCanvas(LEDDOT, 1, (char *)&temp, sizeof(float));
		Delay_ms(100);
		
		/*温度计组件*/
		updateCanvas(TEMPMETER, 0, (char *)&x, sizeof(float));
		Delay_ms(100);
		updateCanvas(TEMPMETER, 1, (char *)&y, sizeof(float));
		Delay_ms(100);
		
		/*标签组件*/
		temp = x+0.5;
		memset(str, 0, sizeof(str));
		sprintf(str, "X:%0.2f", temp);
		updateCanvas(LABEL, 2, str, strlen(str));
		Delay_ms(100);	
		temp = y+0.7;
		memset(str, 0, sizeof(str));
		sprintf(str, "Y:%0.2f", temp);
		updateCanvas(LABEL, 3, str, strlen(str));
		Delay_ms(100);	
		
		/*LED状态组件*/
		temp = (int)x%5;
		updateCanvas(LEDSTATE, 0, (char *)&temp, sizeof(float));
		Delay_ms(100);	
		updateCanvas(LEDSTATE, 1, (char *)&temp, sizeof(float));
		Delay_ms(100);
		
		/*航空仪表盘组件*/
		float buffer[2] = {x, y};
		updateCanvas(PLANEDASHBOARD, 0, (char *)buffer, sizeof(buffer));
		Delay_ms(100);
		updateCanvas(PLANEDASHBOARD, 1, (char *)buffer, sizeof(buffer));
		Delay_ms(100);	
		
		Delay_ms(1000);	
		
		x += 3;
		y += 3;
		if(x>=100)
		{
			x = 0;
		}
		if(y>=100)
		{
			y = 0;
		}
	}
}

