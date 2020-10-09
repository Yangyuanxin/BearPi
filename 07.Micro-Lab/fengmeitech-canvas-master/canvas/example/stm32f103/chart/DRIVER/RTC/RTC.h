/******************************************************************
 * 文件：RTC.h
 * 功能：声明RTC函数
 * 日期：2018-03-26
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#ifndef __RTC_H
#define __RTC_H
#include "stm32f10x.h"

/*日历年份范围，建议其差值小于135年*/
#define FIRST_YEAR 1990
#define LAST_YEAR  2099

/*平闰年枚举*/
typedef enum
{
	COMMON_YEAR = 0,  //平年
	LEAP_YEAR   = 1	  //闰年
}YEARTYPE_ENUM;

/*星期枚举*/
typedef enum
{
	Sun  = 0,   //周日
	Mon  = 1,   //周一
	Tues = 2,   //周二
	Wed  = 3,   //周三
	Thur = 4,   //周四
	Fri  = 5,   //周五
	Sat  = 6    //周六
}WEEK_ENUM;

/*闹钟使能枚举*/
typedef enum
{
    ALARM_OFF = 0x00,   //关闭闹钟功能
    ALARM_ON = 0x01     //开启闹钟功能
}ALARM_ENUM;

/*配置模式枚举*/
typedef enum
{
    NORMAL_MODE = 0x00, //啥也不干
    ALARM_MODE = 0x01,  //闹钟配置模式
    TIME_MODE = 0x02    //日期/时间配置模式
}CONFIGMODE_ENUM;

/*日历结构体*/
typedef struct 
{
	u16 Year;           //年
	u8  Month;          //月
	u8  Day;            //日
	u8  Hour;           //时
	u8  Minute;         //分
	u8  Second;         //秒
	u8  Week;           //星期

    u8 AlarmEN;         //闹钟使能
    u8 AlarmHour;       //闹钟小时部分
    u8 AlarmMinute;     //闹钟分钟部分
}Calendar_Structure;

/*与硬件相关的RTC驱动函数*/
u8 initRTC(void);
u32 getRTCConter(void);
void setRTCConter(u32 conter);
void setRTCAlarmConter(u32 conter);

/*与硬件无关的日历功能函数*/
void setCalendar(Calendar_Structure* pcalendar);
void getCalendar(Calendar_Structure* pcalendar);
void setAlarm(Calendar_Structure* pcalendar);
void updateAlarm(Calendar_Structure* pcalendar);
void enableAlarm(ALARM_ENUM alarm_en,Calendar_Structure* pcalendar);
void showCalendar(Calendar_Structure* pcalendar);
void configMode(CONFIGMODE_ENUM mode,Calendar_Structure* pcalendar);

/*RTC中断服务函数*/
void RTC_IRQHandler(void);

#endif
