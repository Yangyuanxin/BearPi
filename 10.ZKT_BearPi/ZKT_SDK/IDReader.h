#ifndef __IDREADER_H
#define __IDREADER_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "usart.h"

#define swap16(x) ((x&0XFF)<<8|(x&0XFF00)>>8)		//高低字节交换宏定义

/*求表的长度*/
#define NR(x) sizeof(x)/sizeof(x[0])
/*发送命令长度*/
#define CMD_LEN 10
/*命令头长度*/
#define CMD_HEAD_LEN 5
/*接收最大长度*/
#define RECV_MAX_LEN 1292

/*数据包头 0xAA 0xAA 0xAA 0x96 0x69*/
#define CMD_HEADER_0 0xAA
#define CMD_HEADER_1 0xAA
#define CMD_HEADER_2 0xAA
#define CMD_HEADER_3 0x96
#define CMD_HEADER_4 0x69

/*命令和参数*/
#define CMD_RESET_SAM_A 	  	   0x01
#define CMD_RESET_SAM_A_PARA 	   0xFF

#define CMD_READ_SAM_A_STATUS 	   0x11
#define CMD_READ_SAM_A_STATUS_PARA 0xFF

#define CMD_FIND_ID_CARD	  	   0x20
#define CMD_FIND_ID_CARD_PARA	   0x01

#define CMD_SELECT_ID_CARD    	   0x20
#define CMD_SELECT_ID_CARD_PARA    0x02

#define CMD_READ_INFO		  	   0x30
#define CMD_READ_INFO_PARA		   0x01

/*业务终端通过业务终端接口发送的命令集数据结构*/
typedef struct
{
    /*命令*/
    uint8_t CMD ;
    /*命令参数*/
    uint8_t CMD_PARA ;
} BUSSINESS_LIST ;

/*接收基础信息解析*/
typedef struct
{
    uint8_t ID_Card_Name[36];
    uint8_t ID_Card_Number[18];
} ID_Card_Parse  ;

uint8_t Reset_SAM_A(void);
uint8_t Read_SAM_A_Status(void);
uint8_t Find_Id_Card(void);
uint8_t Select_Id_Card(void);
uint8_t Read_Id_Card_Info(void);


uint8_t chr2hex(uint8_t chr);
void unistr2gbkstr(void* dst, const void* src);
uint16_t ff_convert (uint16_t src, uint32_t	dir);






#endif //__IDREADER_H

