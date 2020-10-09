/******************************************************************
 * 文件：RTC.c
 * 功能：实现RTC函数
 * 日期：2018-03-26
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "RTC/RTC.h"
#include "OLED/OLED.h"
#include "math.h"

extern u8 showsta;

Calendar_Structure calendar;

const u8 MonthTable[2][12] = 
{
	{31,28,31,30,31,30,31,31,30,31,30,31}, //平年月份天数
	{31,29,31,30,31,30,31,31,30,31,30,31}  //闰年月份天数
};

/******************************************RTC相关函数*******************************************************/
/**
 * 功能：初始化RTC
 * 参数：None
 * 返回值：是否是第一次使用RTC,非0是第一次,0则不是
 *        程序员可以根据返回值来判断是否需要设置时间
 */
u8 initRTC(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);    //使能PWR和BKP时钟

    PWR_BackupAccessCmd(ENABLE);                                                //BKP访问使能

    if(BKP_ReadBackupRegister(BKP_DR1) != 0x464D)                               //判断是否为第一次使用RTC
    {
        BKP_DeInit();                                                           //设置BKP寄存器回复默认初始值，该行可不加

        RCC_LSEConfig(RCC_LSE_ON);                                              //使能LSE
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);                    //等待LSE稳定

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                                 //设置LSE作为RTC时钟源

        RCC_RTCCLKCmd(ENABLE);                                                  //开启RTC时钟             

        RTC_WaitForSynchro();                                                   //等待RTC寄存器同步完成(RSF)

        RTC_WaitForLastTask();                                                  //等待上次写操作完成(RTOFF)

        RTC_ITConfig(RTC_IT_SEC, ENABLE);                                       //使能RTC秒中断，用于更新时间和刷新屏幕

        RTC_WaitForLastTask();                                                  //等待上次写操作完成(RTOFF)

        RTC_SetPrescaler(32767);                                                //设置预分频值为1S

        RTC_WaitForLastTask();                                                  //等待上次写操作完成(RTOFF)

//        BKP_WriteBackupRegister(BKP_DR1, 0x464D);                               //写上非第一标识

        return 1;
    }else 
    {
        RTC_WaitForLastTask();                                                  //等待上次写操作完成(RTOFF)

        RTC_ITConfig(RTC_IT_SEC, ENABLE);                                       //使能RTC秒中断，用于更新时间和刷新屏幕

        RTC_WaitForLastTask();                                                  //等待上次写操作完成(RTOFF)    

        return 0;
    }



}

/**
 * 功能：获取RTC计数器中的值，通常用于转换成日期和时间
 * 参数：None
 * 返回值：RTC 32位计数器中的值
 */
u32 getRTCConter(void)
{
    return RTC_GetCounter();
}

/**
 * 功能：设置RTC计数器中的值，通常用于设置日期
 * 参数：
 *      conter:待设置值
 * 返回值：None
 */
void setRTCConter(u32 conter)
{
    RTC_WaitForLastTask(); 
    RTC_SetCounter(conter);
    RTC_WaitForLastTask(); 
}

/**
 * 功能：设置闹钟寄存器中的值
 * 参数：
 *       conter:待设置值
 * 返回值：None
 */
void setRTCAlarmConter(u32 conter)
{
    RTC_WaitForLastTask(); 
    RTC_SetAlarm(conter);
    RTC_WaitForLastTask(); 
}


/******************************************日历相关函数*******************************************************/
 
/**
* 功能：根据给定日期计算星期
* 参数：
*	     pcalendar 日历结构体指针
* 返回值：当前星期,星期日-星期六对应为 0-6
* 说明：该算法原理来自于Zeller(蔡勒)公式:w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
*       w:星期值(最后还需对7取余) c:世纪-1 y:年份 m:月份 d:日期
* 补充:
*       1. []内表示取整数，不存在四舍五入，只要是小数就被舍掉
*       2. m取值为3到14，所以1月和2月按去年的13月和14月算,
*          因此当m为1或2时，对应的世纪和年份也要减1
*       3. 该函数计算的结构除了返回之外，也将星期存入到了日历结构中  
*       4. 该函数适合计算1582年10月15日之后的星期值             
*/
static WEEK_ENUM getWeek(Calendar_Structure* pcalendar) 
{
	u8 week;
	u8 century;
	u8 year;
	u8 month;     	
	u8 day;
	
	century = pcalendar->Year/100; 	//得到世纪 
	year = pcalendar->Year%100;	  	//得到年份  
	if(pcalendar->Month<3)			//得到月份 
	{
		month = pcalendar->Month + 12;
		century -= 1;
		if(year==00)
		{
			year = 99;	
		} 
	}else 
	{
		month = pcalendar->Month; 
	}
	day = pcalendar->Day;	       //得到日期
	
	/*蔡勒公式计算星期*/
	week = (u8)(year + year/4 + century/4 - 2*century + 2.6*(month+1)+day-1)%7;
	
	pcalendar->Week = week;
	
	return week;	 
}

/**
* 功能：根据给定日年份计算年份类型(平年或闰年) 
* 参数：
*	     year 待计算的年份
* 返回值：平闰年类型  
* 说明：能被4整除但是不能被100整除的或者能被400整除的都是闰年        
*/
static YEARTYPE_ENUM getYearType(u16 year)
{
	if(((year%4==0) && (year%100!=0)) || (year%400==0))
	{
		return LEAP_YEAR;               //闰年
	}else 
	{
		return COMMON_YEAR;             //平年
	}
}

/**
* 功能：拆分数字各位(个十百千万...)
*       例如：可以将数字1245拆分为1,2,4,5
* 参数：
*	     buffer: 拆分后的各位存储数组
*        number: 待拆分数字
*        digits: 想要拆分的位数，比如当number为12345时
*                该参数应为5，如果小于5会截去高位
*        offset: 结果偏置,该参数主要用于设置得到的结果是数字还是字符
*                如果想要数字，该参数为0,如果想要得到字符该参数为'0'
* 返回值：None 
* 说明：该函数提供了简单直接的求数字各位的功能,唯一需要注意的是：程序员提供的buffer数组大小要大于等于digit
*       否则会出现数组越界,导致跳到STM32的硬件错误中断服务函数并使单片机卡死   
*/
static void splitNumber(u8* buffer,u32 number,u8 digits,u8 offset)
{
	u8 i = 0;
	
	while(digits)
	{
		buffer[i++] = number%(u32)pow(10,digits)/(u32)pow(10,--digits)+offset;
	}
}

/**
* 功能：读取用户通过串口发送过来的数据并将其转换为时间相关单位
* 参数：
*	     mode: 需要程序员指定转换的对象类型:日期/时间或者闹钟
*        pcalendar: 日历结构体指针
* 返回值：None 
*/
static void scanUserInput(CONFIGMODE_ENUM mode,Calendar_Structure* pcalendar)
{
    u8 recbuffer[14];
    u8 i = 0;

    /*清除接收空闲标志位和接收非空标志位*/
    USART_GetFlagStatus(USART1,USART_FLAG_IDLE);
    USART_ReceiveData(USART1);

    if(mode==TIME_MODE)                                                     //设置日期/时间
    {
        while((USART_GetFlagStatus(USART1,USART_FLAG_IDLE)==RESET) && i<14)
        {
            while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
            recbuffer[i++] = USART_ReceiveData(USART1);
        }

        for(i=0;i<14;++i)                                                   //将字符转换为对应数字,如:'1'-'0' = 1
        {
            recbuffer[i] -= '0'; 
        }

        /*赋值给对象结构成员*/
        pcalendar->Year = recbuffer[0]*1000 + recbuffer[1]*100 + recbuffer[2]*10 + recbuffer[3];
        pcalendar->Month = recbuffer[4]*10 + recbuffer[5];
        pcalendar->Day = recbuffer[6]*10 + recbuffer[7];
        pcalendar->Hour = recbuffer[8]*10 + recbuffer[9];
        pcalendar->Minute = recbuffer[10]*10 + recbuffer[11];
        pcalendar->Second = recbuffer[12]*10 + recbuffer[13];
    }else if(mode==ALARM_MODE)                                               //设置闹钟
    {
        while((USART_GetFlagStatus(USART1,USART_FLAG_IDLE)==RESET) && i<4)
        {
            while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
            recbuffer[i++] = USART_ReceiveData(USART1);
        }

        for(i=0;i<4;++i)
        {
            recbuffer[i] -= '0'; 
        }

        /*赋值给对象结构成员*/
        pcalendar->AlarmHour = recbuffer[0]*10 + recbuffer[1];             
        pcalendar->AlarmMinute = recbuffer[2]*10 + recbuffer[3];
    }else 
    {
        return ;
    }    
}

/**
* 功能：设置日期,根据日历结构中的内容转换成RTC计数器中的值
* 参数：
*        pcalendar: 日历结构体指针
* 返回值：None 
*/
void setCalendar(Calendar_Structure* pcalendar)
{
    u32 rtc_conter = 0;
    u16 i;

    /*错误参数检测*/
    if(pcalendar->Year>LAST_YEAR || pcalendar->Year<FIRST_YEAR)
    {
        pcalendar->Year = 1990;
    }
    if(pcalendar->Month>12 || pcalendar->Month<1)
    {
        pcalendar->Month = 1;
    }
    if(pcalendar->Day>31 && pcalendar->Day<1)
    {
        pcalendar->Day = 1;
    }
    if(pcalendar->Hour>23)
    {
        pcalendar->Hour = 23;
    }
    if(pcalendar->Minute>59)
    {
        pcalendar->Minute = 59;
    }
    if(pcalendar->Second>59)
    {
        pcalendar->Second = 59;
    }

    /*计算年份对应秒数*/
    for(i=FIRST_YEAR;i<pcalendar->Year;++i)
    {
        if(getYearType(i)==COMMON_YEAR)                             //如果是平年,则一年有365天
        {
            rtc_conter += 365*24*60*60;
        }else                                                       //如果是闰年,则一年有366天
        {
            rtc_conter += 366*24*60*60;
        }
    }

    /*计算月份对应秒数*/
    for(i=0;i<pcalendar->Month-1;++i)
    {
        if(getYearType(pcalendar->Year)==COMMON_YEAR)               //平年月份天数表
        {
            rtc_conter += MonthTable[COMMON_YEAR][i]*24*60*60;
        }else 
        {
            rtc_conter += MonthTable[LEAP_YEAR][i]*24*60*60;        //闰年月份天数表
        }
    }

    /*计算天数对应秒数*/
    rtc_conter += (pcalendar->Day-1)*24*60*60;
    
    /*计算小时对应秒数*/
    rtc_conter += pcalendar->Hour*60*60;

    /*计算分钟对应秒数*/
    rtc_conter += pcalendar->Minute*60;

    /*计算秒*/
    rtc_conter += pcalendar->Second+1;

    /*更新星期*/
    getWeek(pcalendar);                                             

    /*设置生效*/
    setRTCConter(rtc_conter);
}

/**
* 功能：获取最新时间,根据RTC计数器中的值计算出各个时间参数
* 参数：
*        pcalendar: 转换后的数据存储到日历结构体指针指向的结构体
* 返回值：None 
* 说明: 建议在秒中断中使用,如果用户主动调用,建议最少不低于1/2的采集速度
*/
void getCalendar(Calendar_Structure* pcalendar)
{
    u32 rtc_conter = getRTCConter();
    u16 i;

    /*根据秒数计算年份*/
    for(i=FIRST_YEAR;i<LAST_YEAR;++i)                               //减到不满一年,剩下的就是月份
    {
        if(getYearType(i)==COMMON_YEAR)                             
        {
            if(rtc_conter>365*24*60*60)                             
            {
                rtc_conter -= 365*24*60*60;
            }else 
            {
                break;
            }
            
        }else                                                       
        {
            if(rtc_conter>366*24*60*60)                             
            {
                rtc_conter -= 366*24*60*60;
            }else 
            {
                break;
            }
        }
    }
    pcalendar->Year = i;

    /*根据秒数计算月份*/
    for(i=0;i<12;++i)                                               //减到不满一个月,剩下的就是天数
    {
        if(getYearType(pcalendar->Year)==COMMON_YEAR)
        {
            if(rtc_conter>MonthTable[COMMON_YEAR][i]*24*60*60)     
            {
                rtc_conter -= MonthTable[COMMON_YEAR][i]*24*60*60;
            }else 
            {
                break;
            }
        }else 
        {
            if(rtc_conter>MonthTable[LEAP_YEAR][i]*24*60*60)
            {
                rtc_conter -= MonthTable[LEAP_YEAR][i]*24*60*60;
            }else 
            {
                break;
            }
        }
    }
    pcalendar->Month = i+1;

    /*根据秒数计算天数*/
    for(i=0;i<31;++i)                                               //减到不满一天,剩下的就是小时
    {
        if(rtc_conter>24*60*60)
        {
            rtc_conter -= 24*60*60;
        }else 
        {
            break;
        }
    }
    pcalendar->Day = i+1;

    /*根据秒数计算小时*/
    for(i=0;i<24;++i)                                               //减到不满一小时,剩下的就是分钟
    {
        if(rtc_conter>60*60)
        {
            rtc_conter -= 60*60;
        }else
        {
            break;
        }
    }
    pcalendar->Hour = i;

    /*根据秒数计算分钟*/
    for(i=0;i<60;++i)                                                //减到不满一分钟,剩下的就是秒数
    {
        if(rtc_conter>60)
        {
            rtc_conter -= 60;
        }else 
        {
            break;
        }
    }
    pcalendar->Minute = i;

    pcalendar->Second = rtc_conter-1;                               //剩余的是秒数

    getWeek(pcalendar);                                             //更新星期
}

/**
* 功能：更新最新闹钟值,为了每天都可以在相同时刻进行闹钟,因此我们需要在
*		新的一天到来时对闹钟寄存器中的值进行更新
* 参数：
*        pcalendar: 日历结构体指针
* 返回值：None 
* 说明：由于闹钟寄存器只能写不能读，因此我们使用了备份区寄存器存储我们的闹钟时间,
*       备份区寄存器2,3,4一次存储闹钟小时、闹钟分钟以及闹钟使能
*/
void updateAlarm(Calendar_Structure* pcalendar)
{
	static u8 day = 0;
	
	if(pcalendar->Day != day)
	{
		day = pcalendar->Day;
		
		pcalendar->AlarmHour = BKP_ReadBackupRegister(BKP_DR2);
		pcalendar->AlarmMinute = BKP_ReadBackupRegister(BKP_DR3);
		pcalendar->AlarmEN = BKP_ReadBackupRegister(BKP_DR4);
		
		setAlarm(pcalendar);
	}  
}
/**
* 功能：设置闹钟,将时间参数转换为闹钟寄存器中的值
* 参数：
*        pcalendar: 日历结构体指针
* 返回值：None 
*/
void setAlarm(Calendar_Structure* pcalendar)
{
    u32 alarm_conter = 0;
    u16 i;

    /*错误参数检测*/
    if(pcalendar->AlarmHour>23)
    {
        pcalendar->AlarmHour = 23;
    }
    if(pcalendar->AlarmMinute>59)
    {
        pcalendar->AlarmMinute = 59;
    }

    /*计算年份对应秒数*/
    for(i=FIRST_YEAR;i<pcalendar->Year;++i)
    {
        if(getYearType(i)==COMMON_YEAR)                             //如果是平年,则一年有365天
        {
            alarm_conter += 365*24*60*60;
        }else                                                       //如果是闰年,则一年有366天
        {
            alarm_conter += 366*24*60*60;
        }
    }

    /*计算月份对应秒数*/
    for(i=0;i<pcalendar->Month-1;++i)
    {
        if(getYearType(pcalendar->Year)==COMMON_YEAR)               //平年月份天数表
        {
            alarm_conter += MonthTable[COMMON_YEAR][i]*24*60*60;
        }else 
        {
            alarm_conter += MonthTable[LEAP_YEAR][i]*24*60*60;        //闰年月份天数表
        }
    }

    /*计算天数对应秒数*/
    alarm_conter += (pcalendar->Day-1)*24*60*60;
    
    /*计算小时对应秒数*/
    alarm_conter += pcalendar->AlarmHour*60*60;

    /*计算分钟对应秒数*/
    alarm_conter += pcalendar->AlarmMinute*60;

    /*计算秒,闹钟只设置到整分*/
    alarm_conter += 1;

    /*设置生效*/
    setRTCAlarmConter(alarm_conter);

    BKP_WriteBackupRegister(BKP_DR2, pcalendar->AlarmHour);
    BKP_WriteBackupRegister(BKP_DR3, pcalendar->AlarmMinute);
}


/**
* 功能：使能闹钟
* 参数：
*        alarm_en: 闹钟使能状态 ALARM_ON开闹钟,ALARM_OFF关闹钟
* 返回值：None 
*/
void enableAlarm(ALARM_ENUM alarm_en,Calendar_Structure* pcalendar)
{
    pcalendar->AlarmEN = alarm_en;
    BKP_WriteBackupRegister(BKP_DR4, pcalendar->AlarmEN);
}

/**
* 功能：显示日历各个时间参数
* 参数：
*        pcalendar: 日历结构体指针
* 返回值：None 
*/
void showCalendar(Calendar_Structure* pcalendar)
{
    u8 i;

    /*日期各位拆分后的结果存放数组,一位对应一个数组*/
    u8 year[4];
    u8 month[2];
    u8 day[2];
    u8 hour[2];
    u8 minute[2];
    u8 second[2];


    u8 alarm_hour[2];
    u8 alarm_minute[2];

    /*星期对应字符串*/  
    u8* weekstr[7] = {"Sun ","Mon ","Tues","Wed ","Thur","Fri ","Sat "};

    /*数据拆分开始,拆分结果为字符,方便OLED显示*/
    splitNumber(year,pcalendar->Year,sizeof(year),'0');
    splitNumber(month,pcalendar->Month,sizeof(month),'0');
    splitNumber(day,pcalendar->Day,sizeof(day),'0');
    splitNumber(hour,pcalendar->Hour,sizeof(hour),'0');
    splitNumber(minute,pcalendar->Minute,sizeof(minute),'0');
    splitNumber(second,pcalendar->Second,sizeof(second),'0');

    splitNumber(alarm_hour,pcalendar->AlarmHour,sizeof(alarm_hour),'0');
    splitNumber(alarm_minute,pcalendar->AlarmMinute,sizeof(alarm_minute),'0');

    /*第一行显示 年/月/日 e.g.:2018/03/27*/
    for(i=0;i<sizeof(year);++i)
    {
        showChar(i*FONT_16_EN,0,year[i],FONT_16_EN);
    }
    showChar(i*FONT_16_EN,0,'/',FONT_16_EN);
    for(i=0;i<sizeof(month);++i)
    {
        showChar(i*FONT_16_EN+40,0,month[i],FONT_16_EN);
    }
    showChar(i*FONT_16_EN+40,0,'/',FONT_16_EN); 
    for(i=0;i<sizeof(day);++i)
    {
        showChar(i*FONT_16_EN+64,0,day[i],FONT_16_EN);
    } 

    /*第二行显示 时:分:秒 e.g.:23:05:23*/
    for(i=0;i<sizeof(hour);++i)
    {
        showChar(i*FONT_16_EN,2,hour[i],FONT_16_EN);
    }
    showChar(i*FONT_16_EN,2,':',FONT_16_EN);
    for(i=0;i<sizeof(minute);++i)
    {
        showChar(i*FONT_16_EN+24,2,minute[i],FONT_16_EN);
    }
    showChar(i*FONT_16_EN+24,2,':',FONT_16_EN); 
    for(i=0;i<sizeof(second);++i)
    {
        showChar(i*FONT_16_EN+48,2,second[i],FONT_16_EN);
    }

    /*第三行显示星期 e.g.:Tues*/
    showString(0,4,weekstr[pcalendar->Week],FONT_16_EN);

    /*第四行显示闹钟时间以及是否开启 e.g.:08:30   on*/
    for(i=0;i<sizeof(alarm_hour);++i)
    {
        showChar(i*FONT_16_EN,6,alarm_hour[i],FONT_16_EN);
    }
    showChar(i*FONT_16_EN,6,':',FONT_16_EN);
    for(i=0;i<sizeof(alarm_minute);++i)
    {
        showChar(i*FONT_16_EN+24,6,alarm_minute[i],FONT_16_EN);
    }
    if(pcalendar->AlarmEN==ALARM_ON)
    {
        showString(60,6,"on ",FONT_16_EN);
    }else 
    {
        showString(60,6,"off",FONT_16_EN);
    }
}

/**
* 功能：日历配置模式
* 参数：
*        mode: 指定配置闹钟还是配置日期/时间
*        pcalendar: 配置好后的日期存入该指针指向的日历结构体变量
* 返回值：None 
*/
void configMode(CONFIGMODE_ENUM mode,Calendar_Structure* pcalendar)
{
    if(mode==NORMAL_MODE)       //正常工作模式,通常程序员不会无聊的指定该模式
    {
        return ;
    }else if(mode==TIME_MODE)   //日期/时间设置模式
    {
        printf("\r\n++++++++++++++++++++Time Settings+++++++++++++++++++++++++++++++");
        printf("\r\n  Please input new time, e.g.:20180327130123");
        scanUserInput(TIME_MODE,pcalendar);
        setCalendar(pcalendar);
        printf("\r\n  config completed!");
    }else if(mode==ALARM_MODE)  //闹钟设置模式
    {
        printf("\r\n++++++++++++++++++++Alarm Settings++++++++++++++++++++++++++++++");
        printf("\r\n lease input new Alarm, e.g.:1256");
        scanUserInput(ALARM_MODE,pcalendar);
        setAlarm(pcalendar);
        printf("\r\n config completed!");
    }else                       //其他错误模式       
    {
        return ;
    }
}

/******************************************中断服务函数*******************************************************/
/**
 * 功能：RTC中断服务函数,一般用于每秒更新一次时间以及显示数据
 * 参数：None
 * 返回值：None
 */
void RTC_IRQHandler(void)
{	
	static u8 day = 0;
	if (RTC_GetITStatus(RTC_IT_SEC) == SET)
	{
        getCalendar(&calendar);         //更新时间
		updateAlarm(&calendar);

        if(showsta!=0)
        {
            showCalendar(&calendar);        //显示最新时间
        }
 	}
	
    RTC_WaitForLastTask();	
	RTC_EnterConfigMode();
	RTC_ClearITPendingBit(RTC_IT_SEC);  //软件清除秒中断标志位		
	RTC_ExitConfigMode();
	RTC_WaitForLastTask();	    						 	   	 
}

