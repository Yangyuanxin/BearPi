#ifndef __GUI_APP_H__
#define __GUI_APP_H__
#include "lvgl.h"

//窗体返回事件,这是我们自己定义的一个事件,所定义的值只要不和littleVGL自带的事件值冲突即可
//LV_EVENT_DELETE是在littleVGL中排最后面的一个事件,加10是为了留点余量
#define USER_EVENT_BACK		(LV_EVENT_DELETE+10)  

//函数申明
void gui_app_start(void);
void toast(const char * msg,uint16_t time);
lv_obj_t * general_win_create(lv_obj_t * parent,const char * title);


#endif
