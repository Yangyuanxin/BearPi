 /***********************************************************************************
  * file   : canvas.h
  * brief  : update MicroLab`s canvas
  * author : zx
  * Data   : 2020/4/28-21:58
  * Ver    :
  *             -Ver.1.0 first add 
  * Note   : Developers don't need to care about implementation details, just learn how to use these two function "updateCanvas", "getEvent"
  * BBS    : http://www.openelec.cn/
  * 
  * Copyright (C) 2020-2021 zx. All rights reserved.
  ***********************************************************************************/ 
#ifndef __CANVAS_H
#define __CANVAS_H

#include "protocol.h"

#define CANVASEVENTLIST_SIZE   5

typedef struct
{
    unsigned short componentnumer;
    COMPONENT_TYPE componenttype;
    MSG_TYPE       componentmsgtype;

    char msg[MSGSIZE];
    unsigned short msglen;
}CanvasEvent;

typedef enum 
{
    BLUE    = 0,
    GRAY    = 1,
    GREEN   = 2,
    RED     = 3,
    YELLOW  = 4
}LEDSTYLE;

typedef CanvasEvent CanvasEventList[CANVASEVENTLIST_SIZE];

void initCanvas(CanvasEventList events);
void updateCanvas(COMPONENT_TYPE componenttype, unsigned short componentnumber, char * data, unsigned short datalen);

#endif
