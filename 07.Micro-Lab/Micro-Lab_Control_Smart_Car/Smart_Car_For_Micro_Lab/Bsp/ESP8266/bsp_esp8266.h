#ifndef  __BSP_ESP8266_H__
#define	 __BSP_ESP8266_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include "usart.h"
#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif
/* 类型定义 ------------------------------------------------------------------*/
/******************************* ESP8266 数据类型定义 ***************************/
typedef enum
{
    STA,
    AP,
    STA_AP
} ENUM_Net_ModeTypeDef;

typedef enum
{
    enumTCP,
    enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum
{
    Multiple_ID_0 = 0,
    Multiple_ID_1 = 1,
    Multiple_ID_2 = 2,
    Multiple_ID_3 = 3,
    Multiple_ID_4 = 4,
    Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;

typedef enum
{
    OPEN = 0,
    WEP = 1,
    WPA_PSK = 2,
    WPA2_PSK = 3,
    WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

#define RX_BUF_MAX_LEN     1024                                     //最大接收缓存字节数

typedef struct                                    //串口数据帧的处理结构体
{
    char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
    union
    {
        __IO uint16_t InfAll;
        struct
        {
            __IO uint16_t FramLength       : 15;                              // 14:0
            __IO uint16_t FramFinishFlag   : 1;                               // 15
        } InfBit;
    };
} STRUCT_USARTx_Fram;

/* 宏定义 --------------------------------------------------------------------*/

/******************************** ESP8266 连接引脚定义 ***********************************/

/*********************************************** ESP8266 函数宏定义 *******************************************/
#define ESP8266_Usart( fmt, ... )                      USART_printf (USART2, fmt, ##__VA_ARGS__ )
#define PC_Usart( fmt, ... )                           printf ( fmt, ##__VA_ARGS__ )
//#define PC_Usart( fmt, ... )

//extern UART_HandleTypeDef husartx_esp8266;
extern STRUCT_USARTx_Fram strEsp8266_Fram_Record;
extern uint8_t esp8266_rxdata;



void  ESP8266_Init( void );
void  ESP8266_stop( void );
void  ESP8266_Rst( void );
bool  ESP8266_Cmd( char * cmd, char * reply1, char * reply2, uint32_t waittime );
bool  ESP8266_AT_Test( void );
bool  ESP8266_Net_Mode_Choose( ENUM_Net_ModeTypeDef enumMode );
bool  ESP8266_JoinAP( char * pSSID, char * pPassWord );
bool  ESP8266_BuildAP( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode );
bool  ESP8266_Enable_MultipleId( FunctionalState enumEnUnvarnishTx );
bool  ESP8266_Link_Server( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool  ESP8266_StartOrShutServer( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
uint8_t ESP8266_Get_LinkStatus( void );
uint8_t ESP8266_Get_IdLinkStatus( void );
uint8_t ESP8266_Inquire_ApIp( char * pApIp, uint8_t ucArrayLength );
bool ESP8266_UnvarnishSend( void );
void ESP8266_ExitUnvarnishSend( void );
bool ESP8266_SendString( FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId );
char * ESP8266_ReceiveString( FunctionalState enumEnUnvarnishTx );

void  USART_printf(USART_TypeDef * USARTx, char * Data, ... );








#endif //__BSP_ESP8266_H__

