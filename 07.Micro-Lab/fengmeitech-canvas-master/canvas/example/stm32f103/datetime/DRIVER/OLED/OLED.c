/******************************************************************
 * 文件：OLED.c
 * 功能：实现OLED驱动函数
 * 日期：2018-03-05
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "stdio.h"
#include "string.h"
#include "OLED/OLED.h"
#include "OLED/FONT.h"  	 
#include "DELAY/Delay.h"
#include "IIC/IIC.h"
#include "OLED/BMP.h"
/**********************************静态功能函数**************************************/
/**
 * 功能：根据指定坐标值生效坐标设置
 * 参数：
 * 		x:x轴坐标
 * 		y:y轴坐标
 * 返回值：None
 */
static void setPos(unsigned char x, unsigned char y) 
{ 
	writeCommand(0xb0+y);
	writeCommand(((x&0xf0)>>4)|0x10);
	writeCommand(x&0x0f); 
} 

/**
 * 功能：查找指定汉字在字库中的位置
 * 参数：
 *      str:待查找汉字字符串，一个汉字也是字符串（占用3字节）
 * 		cnfont_index:待查找中文字库索引数组地址
 * 返回值：None
 */
static u8 findCNIndex(u8* str,u8* cnfont_index)
{
	u16 cnfont_size = strlen(cnfont_index);

	u8 index = 0;
	for(index=0;index<cnfont_size/3;++index)
	{
		if(((str[0]^cnfont_index[index*3+0])||(str[1]^cnfont_index[index*3+1])||(str[2]^cnfont_index[index*3+2]))==0)//匹配到汉字索引
		{
			return index;
		}
	}

	return 0; //没有匹配到直接返回字库第一个索引，这里是“风”
}

/**
 * 功能：写入命令给OLED
 * 参数：
 * 		cmd:命令
 * 返回值：None
 */
static void writeCommand(unsigned char cmd)
{
	startIIC();
	sendIICByte(0x78); //发送从机地址及写指令位('0')       
	waitAck();	
	sendIICByte(0x00); //写入控制字节
	waitAck();	
	sendIICByte(cmd); 
	waitAck();	
	stopIIC();
}

/**
 * 功能：写入数据给OLED
 * 参数：
 * 		data:数据
 * 返回值：None
 */
static void writeData(unsigned char data)
{
	startIIC();
	sendIICByte(0x78);	//发送从机地址及写指令位('0')
	waitAck();	
	sendIICByte(0x40);	//写入控制字节
	waitAck();	
	sendIICByte(data);
	waitAck();	
	stopIIC();
}
/**********************************屏幕设置函数**************************************/
/**
 * 功能：设置屏幕反色  
 * 参数：
 * 		set：设置参数   SCREEN_NORMAL,SCREEN_REVERSE可选
 * 返回值：None
 */
void setScreenReverse(SCREEN_SHOW set)
{
	if(set==SCREEN_REVERSE)		//屏幕反色
	{
		writeCommand(0xA7);
	}else 						//屏幕常色
	{
		writeCommand(0xA6);
	}
}

/**
 * 功能：设置屏幕显示方向，类似于手机屏幕翻转  
 * 参数：
 * 		set：设置参数   SCREEN_UP,SCREEN_DOWN可选
 * 返回值：None
 */
void setScreenDir(SCREEN_DIR set)
{
	if(set==SCREEN_UP)			//屏幕正向
	{
		writeCommand(0xA1);
		writeCommand(0xC8);
	}else 						//屏幕倒向
	{
		writeCommand(0xA0);
		writeCommand(0xC0);
	}
}

/**
 * 功能：设置屏幕是否开启，类似于手机息屏和唤醒
 * 参数：
 * 		set：设置参数   SCREEN_ON,SCREEN_OFF可选
 * 返回值：None
 */
void setScreenSwtich(SCREEN_SWITCH set)
{
	if(set==SCREEN_ON)
	{
		writeCommand(0xAF);
	}else 
	{
		writeCommand(0xAE);
	}
}

/**********************************显示屏驱动函数**************************************/
/**
 * 功能：初始化OLED
 * 参数：None
 * 返回值：None
 */
void initOLED(void)
{ 	 
	writeCommand(0x81); 	//设置亮度
	writeCommand(0xFF); 	//亮度值最大 复位默认0x7F
	writeCommand(0xA1);		//设置段映射方式即设置是否水平翻转 A0表示翻转 通常和C0一起使用
	writeCommand(0xC8);		//设置COM扫描模式即设置是否垂直翻转 C0表示翻转 通常和A0一起使用	
	writeCommand(0x8D);		//电荷泵使能
	writeCommand(0x14);
	
	writeCommand(0xAF);		//开屏幕，默认是关闭的就和没上电一样，所以要手动开启
} 

/**
 * 功能：格式化屏幕，常使用0x00或者0xFF清屏，使用不同数据可以产生不同的条纹
 * 参数：
 * 		format_data：格式化内容，一般清屏会用到0x00或者0xFF
 * 返回值：None
 */
void formatScreen(u8 format_data)  
{  
	u8 x,y;		    
	for(y=0;y<8;++y)  
	{  
		writeCommand(0xb0+y);    //设置页地址（0~7）
		writeCommand(0x00);      //设置显示位置—列低地址
		writeCommand(0x10);      //设置显示位置—列高地址   
		for(x=0;x<128;++x)
		{
			writeData(format_data); 
		}	
	} 
}

/**
 * 功能：显示一个字符到OLED
 * 参数：
 * 		x:x轴坐标 0-127
 * 		y:y轴坐标 0-7
 * 		ch:待显示字符 ASCII字符集
 * 		f_size:字体大小 FONT_8_EN(0608) FONT_16_EN(0816)
 * 返回值：None
 */
void showChar(u8 x,u8 y,u8 ch,FONT_SIZE f_size)
{      	
		u8 index = ch-' ';	
		u8 i;
		
		if(x > 127 || y > 7) 			//参数异常处理
		{
			x = 0;
			y = 0;
		}
		if(f_size == FONT_16_EN)		//如果是16*8点阵
		{
			setPos(x,y);	
			for(i=0;i<8;++i)			//由于是8*16的点阵，因此占用两页，要分成写入，此时写入第一页
			{
				writeData(ANSIC0816[index][i]);
			}
			setPos(x,y+1);				//人为指定下一页地址
			for(i=8;i<16;++i)			//由于是8*16的点阵，因此占用两页，要分成写入，此时写入第二页
			{
				writeData(ANSIC0816[index][i]);
			}
			
		}else if(f_size == FONT_8_EN)	//6*8点阵
		{	
			setPos(x,y);
			for(i=0;i<6;i++)			//6*8点阵，写入一页即可
			{
				writeData(ANSIC0608[index][i]);		
			}	
		}else 
		{
			/*其他字体敬请期待:)*/
		}
}

/**
 * 功能：显示字符串到OLED
 * 参数：
 * 		x:x轴坐标 0-127
 * 		y:y轴坐标 0-7
 * 		str:待显示字符串
 * 		f_size:字体大小 FONT_8_EN(0608) FONT_16_EN(0816)
 * 返回值：None
 */  
void showString(u8 x,u8 y,u8* str,FONT_SIZE f_size)
{
	while(*str)
	{
		showChar(x,y,*str++,f_size);	
		x += f_size;		//增加横坐标，移到下一个汉字位置
	}
}

/**
 * 功能：以八进制/十进制/十六进制显示传入的整形数据
 * 参数：
 * 		x:x轴坐标 0-127
 * 		y:y轴坐标 0-7
 * 		number:待显示整数，支持负数
 * 		radix:选择显示进制，可选OCT/DEC/HEX
 *      ndigit:占用几个字符
 * 		f_size:字体大小 FONT_8_EN(0608) FONT_16_EN(0816)
 * 返回值：None
 * 注意：
 */ 
void showNumber(u8 x,u8 y,s32 number,RADIX radix,u8 ndigit,FONT_SIZE f_size)
{
	u8 i = 0;
	u8 str[25] = {0}; 				//定义数字转字符串的存储buffer

	if(radix==DEC) 					//按十进制存储
	{
		sprintf(str,"%d",number);
	}else if(radix==HEX)			//按十六进制存储
	{
		sprintf(str,"%X",number);
	}else if(radix==OCT)			//按八进制存储
	{
		sprintf(str,"%o",number);
	}else 
	{
		sprintf(str,"%d",number);   //参数错误，按十进制处理
	}

	
	for(i=strlen(str);i<ndigit;++i)
	{
		str[i] = ' ';
	}
	
	i = 0;
	while(str[i])
	{
		showChar(x,y,str[i++],f_size);	
		x += f_size;
	}	
}

/**
 * 功能：显示16*16点阵汉字
 * 参数：
 * 		x:x轴坐标 0-127
 * 		y:y轴坐标 0-7
 * 		str:待显示汉字支持单个汉字和多个汉字
 * 		f_size:字体大小 目前只提供了16*16点阵汉字，如果要用其他大小的汉字添加对应判断即可
 * 			    本函数中该参数无效
 * 返回值：None
 */ 
void showCNString(u8 x,u8 y,u8* str,FONT_SIZE f_size)
{   
	u8 i;
	u8 cn_index;
	u8 count;
	if(x > 127 || y > 7) //参数异常处理
	{
		x = 0;
		y = 0;
	}  

	for(count=0;count<strlen(str)/3;++count)
	{
		cn_index = findCNIndex(str+count*3,CN1616_Index);
		setPos(x+16*count,y);
		for(i=0;i<16;++i)
		{
			writeData(CN1616[cn_index][i]);
		}
		setPos(x+16*count,y+1);
		for(i=16;i<32;++i)
		{
			writeData(CN1616[cn_index][i]);
		}
	} 			    
		
}


/**
 * 功能：在制定区域显示图片
 * 参数：
 * 		x:x轴坐标 0-127
 * 		y:y轴坐标 0-7
 * 		x_len:显示区域横坐标长度 0-128
 *		y_len:显示区域纵坐标长度 0-8
 * 		image_index:图片枚举索引
 * 说明：该函数一般用于显示全屏LOGO，另外灵活运用可以显示PPT切换特效
 * 		如让x_len递增LOGO就会从左到右逐渐显示，其他用法类似
 * 返回值：None
 */ 
void showImage(u8 xpos, u8 ypos,u8 x_len, u8 y_len,IMAGE_INDEX  image_index)
{ 	
	u16 i,j;

	for(i=0;i<y_len;++i)					//页地址控制
	{
		setPos(xpos,ypos++);
		
		for(j=i*128+xpos;j<i*128+x_len;++j) //列地址控制
		{
			switch(image_index)
			{
				case FM_LOGO_ENUM        :writeData(FM_LOGO[j]);        break;
				case BRIGHTNESS_LOGO_ENUM:writeData(BRIGHTNESS_LOGO[j]);break;
				case DIRECT_LOGO_ENUM    :writeData(DIRECT_LOGO[j]);    break;
				case REVERSAL_LOGO_ENUM  :writeData(REVERSAL_LOGO[j]);  break;

				default                  :                              break;
			}
			
		}
	}
} 
					    
 





























