#include "gui_app.h"
#include "login_win.h"


//启动GUI系统
void gui_app_start()
{
	//1.创建并使用主题
	//使用不同的主题时,请先把在lv_conf.h配置文件中的相应宏给使能了
	lv_theme_t * theme = lv_theme_night_init(210,NULL);
	lv_theme_set_current(theme);
	
	//2.打开登录窗体
	login_win_create(lv_scr_act());
}


//类似于android系统中的toast提示
//msg:提示消息
//time:多长后自动关闭消息,单位ms
void toast(const char * msg,uint16_t time)
{
	lv_obj_t * parent = lv_scr_act();
	lv_obj_t * mbox = lv_mbox_create(parent,NULL);
	lv_obj_set_width(mbox,lv_obj_get_width(parent)*0.7f);//设置宽度
	lv_label_set_recolor(((lv_mbox_ext_t*)lv_obj_get_ext_attr(mbox))->text,true);//使能文本内容的重绘色功能
	lv_mbox_set_text(mbox,msg);//设置消息
	lv_obj_align(mbox,NULL,LV_ALIGN_IN_TOP_MID,0,30);//设置顶部对齐
	lv_mbox_start_auto_close(mbox,time);//多长时间后自动关闭
}


#define BACK_BTN_TITLE		LV_SYMBOL_LEFT" Back\n"   //返回按钮的文本内容,加\n是为了加大点击区域
//返回按钮专用的事件回调函数
static void back_btn_event_handler(lv_obj_t * obj,lv_event_t event)
{
	if(event==LV_EVENT_PRESSED)//按下事件
	{
		lv_label_set_text(obj,"#444444 "BACK_BTN_TITLE"#");//使能文本颜色变灰,有按下的效果
		lv_label_set_recolor(obj,true);//使能重绘色
	}else if(event==LV_EVENT_RELEASED||event==LV_EVENT_PRESS_LOST)//松手事件或划出控件有效区域事件
	{
		//删除窗体,其实也就是删除lv_page对象
		//因为back_btn是在lv_page页面中的,需要获取2层才能获取到真正的lv_page对象
		obj = lv_obj_get_parent(obj);
		obj = lv_obj_get_parent(obj);//此时的obj为lv_page页面对象,即窗体
		lv_event_cb_t event_cb = lv_obj_get_event_cb(obj);
		if(event_cb)//此窗体是否设置了事件回调函数,设置了的话则调用,可以用来进行释放资源等操作
			event_cb(obj,USER_EVENT_BACK);
		lv_obj_del(obj);//删除此窗体,相当于返回操作 
	}
}

//创建一个通用的窗体,它具有标题和返回按钮
//parent:父对象
//title:窗体的标题
lv_obj_t * general_win_create(lv_obj_t * parent,const char * title)
{
	#define	TOP_OFFSET	5
	lv_obj_t * win;
	
	//创建窗体
	win = lv_page_create(parent,NULL);//用lv_page页面来模拟窗体
	lv_obj_set_size(win,lv_obj_get_width(parent),lv_obj_get_height(parent));//设置窗体的大小
	lv_page_set_sb_mode(win,LV_SB_MODE_AUTO);//设置滚动条的模式为自动
	lv_page_set_edge_flash(win,true);//使能边缘半圆弧动画效果
	lv_page_set_style(win,LV_PAGE_STYLE_SCRL,&lv_style_transp_tight);//设置scrl容器的样式为透明
	//创建一个标题
	lv_obj_t * title_label = lv_label_create(win,NULL);
	lv_label_set_text(title_label,title);//设置窗体标题
	lv_obj_align(title_label,NULL,LV_ALIGN_IN_TOP_MID,0,TOP_OFFSET);//设置对齐方式
	//创建返回按钮
	lv_obj_t * back_btn = lv_label_create(win,NULL);//用标签来模拟按钮
	lv_label_set_text(back_btn,BACK_BTN_TITLE);//设置文本
	lv_obj_align(back_btn,NULL,LV_ALIGN_IN_TOP_LEFT,5,TOP_OFFSET);//设置对齐方式
	lv_obj_set_click(back_btn,true);//使能点击功能
	lv_obj_set_event_cb(back_btn,back_btn_event_handler);//设置事件回调函数,用于关闭窗体
	
	return win;
}









