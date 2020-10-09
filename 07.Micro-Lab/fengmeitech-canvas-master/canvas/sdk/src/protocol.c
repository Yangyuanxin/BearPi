 /***********************************************************************************
  * file   : protocol.c
  * brief  : packaging & unpacking protocol packet
  * author : zx
  * Data   : 2020/4/28-21:58
  * Ver    :
  *             -Ver.1.0 first add 
  * Note   : Developers don't need to care about implementation details, just learn how to use these two function "packProtocol()" & "packProtocol()"
  * BBS    : http://www.openelec.cn/
  * 
  * Copyright (C) 2020-2021 zx. All rights reserved.
  ***********************************************************************************/ 
#include "protocol.h"

/**
 * brief : pack protocol data to tbuffer
 * parameter:
 *      organization : *
 *      section      : *
 *      datapoint    : *
 *      componenttype: component type button/switch/cardashboard ..
 *      componentnum : component number
 *      msgtype      : component emit message type
 *      msg          : msg content
 *      msglen       : msg lenth
 *      tbuffer      : save packing result ， user send tbuffer with uart/net
 *      size         : tbuffer size
 * ret: none
 */
void packProtocol(unsigned short organization, unsigned short section, unsigned short datapoint, COMPONENT_TYPE componenttype, unsigned short componentnum, MSG_TYPE msgtype, char * msg, unsigned short msglen, char * tbuffer, unsigned short * tbufferlen)
{
#if VERSION == VERSION_1_0
    /*check memory overflow*/
    if(PROTOCOL_SIZE+msglen+1>TBUFFERSIZE)
    {
        return ;
    }

    /*packing head*/
    *(unsigned short *)tbuffer = HEAD;

    /*packing version*/
    *(tbuffer + HEAD_SIZE) = VERSION;

    /*packing organization*/
    *(unsigned short *)(tbuffer + HEAD_SIZE + VERSION_SIZE) = organization;

    /*packing section*/
    *(unsigned short *)(tbuffer + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE) = section;

    /*packing datapoint*/
    *(unsigned short *)(tbuffer + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE) = datapoint;

    /*packing component type*/
    *(unsigned short *)(tbuffer + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE) = componenttype;

    /*packing component number*/
    *(unsigned short *)(tbuffer + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE + COMPONENTTYPE_SIZE) = componentnum;

    /*packing component msg lenth*/
    *(unsigned short *)(tbuffer + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE + COMPONENTTYPE_SIZE + COMPONENTNUMBER_SIZE) = msglen;

    /*packing component msg content*/
    for(unsigned char i=0;i<msglen;++i)
    {
        *(tbuffer+PROTOCOL_SIZE-CRC_SIZE+i) = msg[i];
    }

    /*packing crc*/
    unsigned char sum = 0;
    for(unsigned char i=0;i<PROTOCOL_SIZE-CRC_SIZE+msglen;++i)
    {
        sum += (unsigned char)tbuffer[i];
    }

    *(tbuffer + PROTOCOL_SIZE -CRC_SIZE + msglen) = sum;

    *tbufferlen = PROTOCOL_SIZE + msglen;

#endif
}

/**
 * brief : pack protocol data to tbuffer
 * parameter:
 *      protocol     : protocol structure
 *      data         : receive data from uart/net ..
 *      datalen      : data lenth
 * ret: 
 *      0 SUCCESS -1 FAIL
 */
char unpackProtocol(ProtocolObject * protocol, char * data, unsigned short datalen, char * rbuffer,  unsigned short * rbufferlen)
{
    /*check head*/
    if(*(unsigned short *)data!=(unsigned short)HEAD)
    {
        return -1;
    }

    /*check lenth*/
    if(datalen-PROTOCOL_SIZE-COMPONENTMSGTYPE_SIZE > *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE + COMPONENTTYPE_SIZE + COMPONENTNUMBER_SIZE + COMPONENTMSGTYPE_SIZE))
    {
        return -1;
    }

    /*check crc*/
    unsigned char sum = 0;
    for(unsigned char i=0;i<datalen-1;++i)
    {
        sum += (unsigned char)data[i];
    }
    if(sum!=data[datalen-1])
    {
        return -1;
    }

    /*version*/
    if(data[HEAD_SIZE]==0x10)
    {
        /*check memory overflow*/
        if(datalen-PROTOCOL_SIZE>MSGSIZE)
        {
            return -1;
        }

        /*get organization*/
        protocol->organization = *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE);

        /*get section*/
        protocol->section = *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE);

        /*get datapoint*/
        protocol->datapoint = *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE);

        /*get component type*/
        protocol->componenttype = *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE);

        /*get component number*/
        protocol->componentnum = *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE + COMPONENTTYPE_SIZE);

        /*get component message type*/
        protocol->msgtype = *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE + COMPONENTTYPE_SIZE + COMPONENTNUMBER_SIZE);

        /*get component message lenth*/
        *rbufferlen = *(unsigned short *)(data + HEAD_SIZE + VERSION_SIZE + ORGANIZATION_SIZE + SECTION_SIZE + DATAPOINT_SIZE + COMPONENTTYPE_SIZE + COMPONENTNUMBER_SIZE + COMPONENTMSGTYPE_SIZE);       
  
        /*get component message content*/
        for(char i=0;i<datalen-PROTOCOL_SIZE;++i)
        {
            rbuffer[i] = data[PROTOCOL_SIZE-CRC_SIZE+1+i];
        }
    }else 
    {
        /*other version*/
    }

    return 0;
}