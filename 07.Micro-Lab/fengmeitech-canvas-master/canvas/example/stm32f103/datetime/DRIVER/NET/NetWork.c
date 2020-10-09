/******************************************************************
 * 文件：NetWork.c
 * 功能：实现TCP、UDP通信相关函数
 * 日期：2018-04-06
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "string.h"
#include "NetWork.h"
#include "LED/LED.h"
#include "RELAY/Relay.h"
#include "DELAY/Delay.h"

u8 TXBuffer[TXBUFFER_LEN] = {0};  //网络通信数据发送缓冲
u8 RXBuffer[RXBUFFER_LEN] = {0};  //网络通信数据接收缓冲

/**
 * 功能：查找字符串中是否包含另一个字符串
 * 参数：
 *       dest：待查找目标字符串
 *       src：待查找内容
 *       retry_cn:查询超时时间
 * 返回值：查找结果，非0为查找成功,0为失败
 * 说明：
 *       当发出一个AT指令后，需要一段时间等待ESP8266回复,因此就需要等待一段时间,
 *       这个时间通常是几百ms(除了连接服务器和AP指令),本程序一般指令通常等待
 *       2S,耗时的连接AP和服务器的设置等待为8S,其实花不了那么多时间,但如果发生超时
 *       就一定可以判断是通信问题
 */
static u8 findStr(u8* dest,u8* src,u16 retry_cn)
{
    u16 retry = retry_cn;                   //超时时间
    u8 result_flag = 0;                     //查找结果

    while(strstr(dest,src)==0 && --retry!=0)//等待串口接收完毕或超时退出
    {
        Delay_ms(10);
    }

   if(retry==0)                            //如果超时则有问题,此时返回0
   {
       return 0;
   }
   result_flag = 1;                        //执行到这里说明一切正常, 表示查找成功

    if(result_flag)
    {

        return 1;
    }else 
    {
        return 0;
    }
}

/**
 * 功能：初始化ESP8266
 * 参数：None
 * 返回值：初始化结果，非0为初始化成功,0为失败
 */
u8 initESP8266(void)
{
    u8 result = 0;

    Delay_ms(500);
    result = checkESP8266();              //使用AT指令检查ESP8266是否存在

    return result;                         //设置成功

}

/**
 * 功能：恢复出厂设置
 * 参数：None
 * 返回值：None
 * 说明:此时ESP8266中的用户设置将全部丢失回复成出厂状态
 */
void restoreESP8266(void)
{
	sendString(USART2,"+++");           //退出透传
    Delay_ms(500);
	sendString(USART2,"AT+RESTORE\r\n");//恢复出厂 
    NVIC_SystemReset();                 //同时重启单片机   
}

/**
 * 功能：检查ESP8266是否正常
 * 参数：None
 * 返回值：ESP8266返回状态
 *        非0 ESP8266正常
 *        0 ESP8266有问题  
 */
u8 checkESP8266(void)
{
    memset(RXBuffer,0,RXBUFFER_LEN); //清空接收缓冲

    sendString(USART2,"AT\r\n");     //发送AT握手指令

    if(findStr(RXBuffer,"OK",200)!=0)//ESP8266正常
    {
        return 1;  
    }else                            //ESP8266不正常 
    {
        return 0;
    }
}

/**
 * 功能：连接热点
 * 参数：
 *         ssid:热点名
 *         pwd:热点密码
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. WIFI名和密码不正确
 *         2. 路由器连接设备太多,未能给ESP8266分配IP
 */
u8 connectAP(u8* ssid,u8* pwd)
{
    memset(RXBuffer,0,RXBUFFER_LEN);                       
    sendString(USART2,"AT+CWMODE?\r\n");                       //查询此时WIFI工作模式
    if(findStr(RXBuffer,"CWMODE:1",200)==0)                    //如果此时不是MODE1模式,即不是STATION模式
    {
        memset(RXBuffer,0,RXBUFFER_LEN);     
        sendString(USART2,"AT+CWMODE_CUR=1\r\n");              //设置为STATION模式
        if(findStr(RXBuffer,"OK",200)==0)
        {
            return 0;
        }             
    }

    memset(TXBuffer,0,RXBUFFER_LEN);                            //清空发送缓冲
    memset(RXBuffer,0,RXBUFFER_LEN);                            //清空接收缓冲
    sprintf(TXBuffer,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,pwd);//连接目标AP
    sendString(USART2,TXBuffer);
    if(findStr(RXBuffer,"OK",800)!=0)                           //连接成功且分配到IP
    {
        return 1;
    }
}

/**
 * 功能：使用指定协议(TCP/UDP)连接到服务器
 * 参数：
 *         mode:协议类型 "TCP","UDP"
 *         ip:目标服务器IP
 *         port:目标是服务器端口号
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. 远程服务器IP和端口号有误
 *         2. 未连接AP
 *         3. 服务器端禁止添加(一般不会发生)
 */
u8 connectServer(u8* mode,u8* ip,u16 port)
{
    memset(RXBuffer,0,RXBUFFER_LEN);      
    memset(TXBuffer,0,RXBUFFER_LEN);   
   
    sendString(USART2,"+++");                   //多次连接需退出透传
    Delay_ms(500);
    /*格式化待发送AT指令*/    
    sprintf(TXBuffer,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",mode,ip,port);
    sendString(USART2,TXBuffer);
    if(findStr(RXBuffer,"CONNECT",800)!=0)
    {
        memset(RXBuffer,0,RXBUFFER_LEN);    
        sendString(USART2,"AT+CIPMODE=1\r\n");  //设置为透传模式
        if(findStr(RXBuffer,"OK",200)!=0)
        {
            memset(RXBuffer,0,RXBUFFER_LEN); 
            sendString(USART2,"AT+CIPSEND\r\n");//开始处于透传发送状态
            if(findStr(RXBuffer,">",200)!=0)
            {
                return 1;
            }else 
            {
                return 0;
            }    
        }else 
        {
            return 0;
        }
        
    }else 
    {
        return 0;
    }
}

/**
 * 功能：主动和服务器断开连接
 * 参数：None
 * 返回值：
 *         连接结果,非0断开成功,0断开失败
 */
u8 disconnectServer(void)
{
    sendString(USART2,"+++");            //退出透传
    Delay_ms(500);
    memset(RXBuffer,0,RXBUFFER_LEN);  
    sendString(USART2,"AT+CIPCLOSE\r\n");//关闭链接

    if(findStr(RXBuffer,"CLOSED",200)!=0)//操作成功,和服务器成功断开
    {
        return 1;
    }else 
    {
        return 0;
    }
}

/**
 * 功能：透传模式下的数据发送函数
 * 参数：
 *      buffer:待发送数据
 * 返回值：None
 */
void sendBuffertoServer(u8* buffer)
{
    memset(RXBuffer,0,RXBUFFER_LEN);
    sendString(USART2,buffer); 
}

/**
 * 功能：处理服务器发回来的控制指令
 * 参数：None
 * 返回值：None
 */
void processServerBuffer(void)
{ 
    u8 i = 0;

    /*LED状态控制*/
    if(strstr(RXBuffer,"LED_ON"))
    {
        ++i;
        openLED();
    }else if(strstr(RXBuffer,"LED_OFF"))
    {
        ++i;
        closeLED();
    }else 
    {

    }
    
    /*继电器1状态控制*/
    if(strstr(RXBuffer,"RELAY1_CLOSE"))
    {
        ++i;
        setRelay(RELAY1,RELAY_CLOSE);
    }else if(strstr(RXBuffer,"RELAY1_OPEN"))
    {
        ++i;
        setRelay(RELAY1,RELAY_OPEN);
    }else 
    {

    }

    /*继电器2状态控制*/
    if(strstr(RXBuffer,"RELAY2_CLOSE"))
    {
        ++i;
        setRelay(RELAY2,RELAY_CLOSE);
    }else if(strstr(RXBuffer,"RELAY2_OPEN"))
    {
        ++i;
        setRelay(RELAY2,RELAY_OPEN);
    }else 
    {
        
    }  

    /*只在接收控制指令时才清空,这样可避免接收AT指令时导致失败*/
    if(i!=0)
    {
        memset(RXBuffer,0,RXBUFFER_LEN);
    }          
}
