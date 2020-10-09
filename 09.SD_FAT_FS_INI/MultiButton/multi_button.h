/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#ifndef _MULTI_BUTTON_H_
#define _MULTI_BUTTON_H_

#include "stdint.h"
#include "string.h"

//According to your need to modify the constants.
#define TICKS_INTERVAL    5	//ms
#define DEBOUNCE_TICKS    3	//MAX 8
#define SHORT_TICKS       (300 /TICKS_INTERVAL)
#define LONG_TICKS        (1000 /TICKS_INTERVAL)


typedef void (*BtnCallback)(void*);
typedef void (*MenuCallback)(void*);

/*菜单*/
typedef enum
{
    MAIN_PAGE = 0,
    LOG_PAGE,
} MENU;

/*事件值*/
typedef enum
{
    NULL_KEY_EVENT = -1,
    LEFT_KEY_SHORT = 0,
    LEFT_KEY_LONG  = 1,
    ENTER_KEY_SHORT = 2,
    ENTER_KEY_LONG = 3,
    RIGHT_LEY_SHORT = 4,
    RIGHT_KEY_LONG = 5,
    UP_KEY_SHORT = 6,
    UP_KEY_LONG = 7,
    DOWN_KEY_SHORT = 6,
    DOWN_KEY_LONG = 7,
    RETRUN_KEY_SHORT = 8,
    RETRUN_KEY_LONG = 9,
    POWER_KEY_SHORT = 10,
    POWER_KEY_LONG = 11,
} EVENT_CODE;

typedef enum
{
    PRESS_DOWN = 0,
    PRESS_UP,
    PRESS_REPEAT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_RRESS_START,
    LONG_PRESS_HOLD,
    number_of_event,
    NONE_PRESS
} PressEvent;

typedef struct Button
{
    uint16_t ticks;
    uint8_t  repeat : 4;
    uint8_t  event : 4;
    uint8_t  state : 3;
    uint8_t  debounce_cnt : 3;
    uint8_t  active_level : 1;
    uint8_t  button_level : 1;
    uint8_t  (*hal_button_Level)(void);
    BtnCallback  cb[number_of_event];
    struct Button* next;
} Button;


typedef struct Menu
{
    /*当前正在执行的页面*/
    uint8_t Current_Page ;
    /*当前触发的事件*/
    int KeyEvent ;
} Menu ;


#ifdef __cplusplus
extern "C" {
#endif

void button_init(struct Button* handle, uint8_t(*pin_level)(), uint8_t active_level);
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);
PressEvent get_button_event(struct Button* handle);
int  button_start(struct Button* handle);
void button_stop(struct Button* handle);
void button_ticks(void);


/*菜单初始化*/
void menu_init(struct Menu *handle, uint8_t Page, int EVENT_CODE);
/*获取当前菜单*/
uint8_t Get_Menu(struct Menu *handle);
/*获取当前发生的事件值*/
int Get_Event_Code(struct Menu *handle);
/*设置当前发生的事件值*/
void Set_Event_Code(struct Menu *handle, int Event_Code);
/*菜单跳转*/
void Set_Menu(struct Menu *handle, uint8_t Page);


#ifdef __cplusplus
}
#endif

#endif
