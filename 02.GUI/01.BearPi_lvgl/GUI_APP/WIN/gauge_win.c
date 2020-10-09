#include "gauge_win.h"
#include "gui_app.h"
#include <stdio.h>

static lv_obj_t * gauge1;
static lv_obj_t * label1;
static lv_obj_t * lmeter1;
static int16_t speed_val = 0;
lv_color_t needle_colors[2];//每一个指针的颜色
static lv_task_t * task = NULL;

//事件回调函数
static void event_handler(lv_obj_t * obj,lv_event_t event)
{
	if(event==USER_EVENT_BACK)//窗体返回事件,点击左上角的"< Back"按钮会触发此事件
	{
		if(task)//删除掉任务
		{
			lv_task_del(task);
			task = NULL;
		}
	}
}

//任务回调函数
void task_cb(lv_task_t * task)
{
	static uint8_t is_add_dir = 1;//是否是速度增加的方向
	char buff[40];

	if(is_add_dir)
	{
			speed_val += 5;
			if(speed_val>=100)
				is_add_dir = 0;
	}else
	{
			speed_val -= 5;
			if(speed_val<=0)
				is_add_dir = 1;
	}
	//设置指针的数值
	lv_gauge_set_value(gauge1,0,speed_val);
	//把此速度显示在标签上,然后根据不同的数值显示出不同的颜色
	//绿色代表安全,黄色代表警告,红色代表危险  
	sprintf(buff,"#%s %d km/h#",speed_val<60?"00FF00":(speed_val<90?"FFFF00":"FF0000"),speed_val); 
	lv_label_set_text(label1,buff);
	//设置刻度指示器的数值
	lv_lmeter_set_value(lmeter1,speed_val);
}

//创建窗体
void gauge_win_create(lv_obj_t * parent)
{
	lv_obj_t * win;
	win = general_win_create(parent,"Gauge");
	lv_obj_set_event_cb(win,event_handler);//设置事件回调函数
	
	//创建仪表盘
	gauge1 = lv_gauge_create(win, NULL);
	lv_obj_set_size(gauge1,200,200);//设置大小
	lv_gauge_set_range(gauge1,0,100);//设置仪表盘的范围
	needle_colors[0] = LV_COLOR_BLUE;
	needle_colors[1] = LV_COLOR_PURPLE;
	lv_gauge_set_needle_count(gauge1,sizeof(needle_colors)/sizeof(needle_colors[0]),needle_colors);
	lv_gauge_set_value(gauge1,0,speed_val);//设置指针1指向的数值,我们把指针1当作速度指针
	lv_gauge_set_value(gauge1,1,90);//设置指针2指向的数值,就让它指向关键数值点
	lv_gauge_set_critical_value(gauge1,90);//设置关键数值点
	lv_gauge_set_scale(gauge1,240,31,6);//设置角度,刻度线的数量,数值标签的数量
	lv_obj_align(gauge1,NULL,LV_ALIGN_IN_TOP_MID,0,40);//设置对齐方式
	
	//创建一个标签,用来显示仪表盘上指针1的数值
	label1 = lv_label_create(win,NULL);
	lv_label_set_long_mode(label1,LV_LABEL_LONG_BREAK);//设置长文本模式
	lv_obj_set_width(label1,80);//设置宽度
	lv_label_set_align(label1,LV_LABEL_ALIGN_CENTER);//设置文本居中对齐
	lv_label_set_body_draw(label1,true);//使能背景重绘
	lv_obj_align(label1,gauge1,LV_ALIGN_CENTER,0,60);//设置对齐方式
	lv_label_set_text(label1,"0 km/h");//设置文本
	lv_label_set_recolor(label1,true);//使能文本重绘色
	
	//创建一个刻度指示器
	lmeter1 = lv_lmeter_create(win,NULL);
	lv_obj_set_size(lmeter1,150,150);//设置大小
	lv_obj_align(lmeter1,gauge1,LV_ALIGN_OUT_BOTTOM_MID,0,10);//设置对齐方式
	lv_lmeter_set_range(lmeter1,0,100);//设置范围
	lv_lmeter_set_value(lmeter1,speed_val);//设置当前的值
	lv_lmeter_set_scale(lmeter1,240,31);//设置角度和刻度线的数量
	
	
	//创建一个任务来模拟速度指针的变化 
	if(task==NULL)
		task = lv_task_create(task_cb,600,LV_TASK_PRIO_MID,NULL);
}













