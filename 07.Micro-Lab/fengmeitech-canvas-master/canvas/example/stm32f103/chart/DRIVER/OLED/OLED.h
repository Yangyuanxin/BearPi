/******************************************************************
 * ???OLED.h
 * ?????OLED???????
 * ???2018-03-04
 * ???zx
 * ???Ver.1.0 | ????
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __OLED_H
#define __OLED_H			  	  
#include "stm32f10x.h"

/**************************??????*****************************/				   
typedef enum
{
    SCREEN_UP = 0,
    SCREEN_DOWN = 1
}SCREEN_DIR;

/**************************??????*****************************/	
typedef enum
{
    SCREEN_NORMAL = 0,
    SCREEN_REVERSE = 1
}SCREEN_SHOW;


/**************************????/????*****************************/	
typedef enum
{
    SCREEN_ON = 0,
    SCREEN_OFF = 1
}SCREEN_SWITCH;


/**************************??????*****************************/	
typedef enum
{
    FONT_8_EN  = 6,
    FONT_16_EN = 8,
    FONT_16_CN = 16
}FONT_SIZE;


/**************************????*****************************/	
typedef enum 
{
    OCT = 8,
    DEC = 10,
    HEX = 16
}RADIX;


/**************************??????*****************************/	
//?????????????????????????????????????????
typedef enum
{
    FM_LOGO_ENUM = 0,
    DIRECT_LOGO_ENUM = 1,
    BRIGHTNESS_LOGO_ENUM = 2,
    REVERSAL_LOGO_ENUM = 3
}IMAGE_INDEX;

void initOLED(void);

void writeCommand(unsigned char cmd);
void writeData(unsigned char data);

void formatScreen(u8 format_data);

void showChar(u8 x,u8 y,u8 ch,FONT_SIZE f_size);  
void showString(u8 x,u8 y,u8* str,FONT_SIZE f_size);
void showNumber(u8 x,u8 y,s32 number,RADIX radix,u8 ndigit,FONT_SIZE f_size);
void showCNString(u8 x,u8 y,u8* str,FONT_SIZE f_size);

void showImage(u8 xpos, u8 ypos,u8 x_len, u8 y_len,IMAGE_INDEX  image_index);

void setScreenReverse(SCREEN_SHOW set);
void setScreenDir(SCREEN_DIR set);
void setScreenSwtich(SCREEN_SWITCH set);				    

#endif  
	 



