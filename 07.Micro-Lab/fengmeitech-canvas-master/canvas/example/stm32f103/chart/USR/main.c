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
	unsigned short dht11;
	
	/*初始化各外设*/ 
	initSysTick(); 
	initNVIC(NVIC_PriorityGroup_2);
	initUART();
	initLED();
	initDHT11();
	initCanvas(events);
	
	while (1)
	{
		dht11 = readDHT11();  //获取温湿度
	    float temp = dht11&0x00FF;
		float hum = (dht11&0xFF00)>>8;
		/*水位进度组件*/
		updateCanvas(WAVECHART, 0, (char *)&temp, sizeof(float));
		Delay_ms(50);	
		updateCanvas(WAVECHART, 1, (char *)&hum, sizeof(float));
		Delay_ms(1000);

		toggleLED();		
	}
}

