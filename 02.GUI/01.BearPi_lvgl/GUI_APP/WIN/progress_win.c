#include "progress_win.h"
#include "gui_app.h"
#include <stdio.h>

static lv_obj_t * arc1;
static lv_obj_t * progress_label;
static lv_obj_t * bar1;
static lv_task_t * task = NULL;
uint8_t progress_val;

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

//设置进度值
//val:范围为[0,100]
static void progress_set_val(uint8_t val)
{
	char buff[10];
	//更新arc的进度
	lv_arc_set_angles(arc1,0,(uint16_t)(3.6f*val));
	//更新bar的进度
	lv_bar_set_value(bar1,val,LV_ANIM_OFF);
	//更新显示进度值的标签
	sprintf(buff,"%d%%",val);
	lv_label_set_text(progress_label,buff);
	lv_obj_realign(progress_label);
}

//任务回调函数
static void progress_task(lv_task_t * t)
{
	progress_val++;
	if(progress_val>100)
		 progress_val = 0;
	progress_set_val(progress_val);
}

//创建窗体
void progress_win_create(lv_obj_t * parent)
{
	lv_obj_t * win = general_win_create(parent,"Progress");
	lv_obj_set_event_cb(win,event_handler);//设置事件回调函数
	
	//创建预加载对象
  lv_obj_t * preload1 = lv_preload_create(win,NULL);
  lv_obj_set_size(preload1,100,100);//设置大小
  lv_obj_align(preload1,NULL,LV_ALIGN_IN_TOP_MID,0,40);//设置对齐方式
  lv_preload_set_arc_length(preload1,45);//设置小圆弧对应的角度
  lv_preload_set_dir(preload1,LV_PRELOAD_DIR_BACKWARD);//设置为逆时针
  lv_preload_set_type(preload1,LV_PRELOAD_TYPE_FILLSPIN_ARC);//设置旋转动画的方式
  lv_preload_set_spin_time(preload1,2000);//设置旋转的速度,转一圈所需要的时间
	
	//创建弧形
	arc1 = lv_arc_create(win, NULL);
	lv_arc_set_angles(arc1, 180, 90);//设置角度
	lv_obj_set_size(arc1, 100, 100);//设置大小,设置的宽度和高度必须相等,弧形半径等于宽度的一半
	lv_obj_align(arc1,preload1,LV_ALIGN_OUT_BOTTOM_MID,0,15);
	//创建一个用来显示弧形进度的标签
	progress_label = lv_label_create(win,NULL);
  lv_obj_align(progress_label,arc1,LV_ALIGN_CENTER,0,0);
	
	//创建一个水平进度条
	bar1 = lv_bar_create(win, NULL);
	lv_obj_set_size(bar1,180,10);//设置大小,如果宽度比高度小,那就是垂直进度条了
	lv_obj_align(bar1,arc1,LV_ALIGN_OUT_BOTTOM_MID,0,15);
	
	//设置默认进度
	progress_val = 0;
	progress_set_val(progress_val);
	//创建一个任务来模拟进度的加载过程
	if(task==NULL)
		task = lv_task_create(progress_task,400,LV_TASK_PRIO_MID,NULL);
}

















