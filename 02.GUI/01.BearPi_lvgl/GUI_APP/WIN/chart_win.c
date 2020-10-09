#include "chart_win.h"
#include "gui_app.h"


#define POINT_COUNT   	10  //数据线所具有的数据点个数
#define SLIDER_MAX_VAL	100	//滑块的最大值
static lv_coord_t series1_y[POINT_COUNT] = {30,60,40,60,20,60,50,80,60,80};
static lv_obj_t * chart1;
static lv_chart_series_t * series1;
static lv_obj_t * slider1;
static lv_obj_t *  sw1;
static lv_obj_t * cb1;

//事件回调函数
static void event_handler(lv_obj_t * obj,lv_event_t event)
{
	float val;
	uint8_t i;
	bool sta;
	
	if(event==LV_EVENT_VALUE_CHANGED)
	{
		if(obj==slider1)//滑块,用来改变数据线的缩放
		{
			val = lv_slider_get_value(obj);//获取滑块的当前值
			val /= SLIDER_MAX_VAL;//得到缩放系数
			for(i=0;i<POINT_COUNT;i++)
				series1->points[i] = (lv_coord_t)(val*series1_y[i]);//按照缩放系数,修改数据点的值
			lv_chart_refresh(chart1);//更新图表
		}else if(obj==sw1)//开关,用来选择图表的类型
		{
			sta = lv_sw_get_state(obj);
			if(sta)
				lv_chart_set_type(chart1,LV_CHART_TYPE_POINT|LV_CHART_TYPE_LINE);//设置为散点和折线的组合
			else
				lv_chart_set_type(chart1,LV_CHART_TYPE_COLUMN);//柱状图 
		}else if(obj==cb1)//复选框,也是用来选择图表的类型
		{
			sta = lv_cb_is_checked(obj);
			if(sta)
				lv_chart_set_type(chart1,LV_CHART_TYPE_AREA);//面积图
			else
				lv_chart_set_type(chart1,LV_CHART_TYPE_POINT|LV_CHART_TYPE_LINE);//设置为散点和折线的组合
		}
	}
}

//创建窗体
void chart_win_create(lv_obj_t * parent)
{
	lv_obj_t * win;
	win = general_win_create(parent,"Chart");
	
	//创建图表
	chart1 = lv_chart_create(win,NULL);
  lv_obj_set_size(chart1,180,200);//设置大小
  lv_obj_align(chart1,NULL,LV_ALIGN_IN_TOP_MID,17,40);//设置对齐方式
  lv_chart_set_type(chart1,LV_CHART_TYPE_POINT|LV_CHART_TYPE_LINE);//设置为散点和折线的组合
  lv_chart_set_series_opa(chart1,LV_OPA_80);//设置数据线的透明度
  lv_chart_set_series_width(chart1,4);//设置数据线的宽度
  lv_chart_set_series_darking(chart1,LV_OPA_80);//设置数据线的黑阴影效果
  lv_chart_set_point_count(chart1,POINT_COUNT);//设置数据点的个数
  lv_chart_set_div_line_count(chart1,4,4);//设置水平和垂直分割线
  lv_chart_set_range(chart1,0,100);//设置y轴的数值范围
  lv_chart_set_y_tick_length(chart1,10,3);
  lv_chart_set_y_tick_texts(chart1,"100\n80\n60\n40\n20\n0",2,LV_CHART_AXIS_DRAW_LAST_TICK);
  lv_chart_set_x_tick_length(chart1,10,3);
  lv_chart_set_x_tick_texts(chart1,"0\n2\n4\n6\n8\n10",2,LV_CHART_AXIS_DRAW_LAST_TICK);
  lv_chart_set_margin(chart1,40);
  //往图表中添加数据线
  series1 = lv_chart_add_series(chart1,LV_COLOR_RED);//指定为红色
  lv_chart_set_points(chart1,series1,(lv_coord_t*)series1_y);//初始化数据点的值
	
	//创建滑块
	slider1 = lv_slider_create(win,NULL);
	lv_obj_set_size(slider1,200,20);//设置大小
	lv_slider_set_range(slider1,0,SLIDER_MAX_VAL);//设置范围
	lv_slider_set_value(slider1,SLIDER_MAX_VAL,LV_ANIM_OFF);
	lv_obj_align(slider1,chart1,LV_ALIGN_OUT_BOTTOM_MID,-17,45);//设置对齐方式
	lv_obj_set_event_cb(slider1,event_handler);//设置事件回调函数
	
	//创建复选框
	cb1 = lv_cb_create(win, NULL);
	lv_cb_set_text(cb1,"Area type");//设置文本
	lv_cb_set_checked(cb1,false);//设置复选框默认没有被选中 
	lv_obj_align(cb1,slider1,LV_ALIGN_OUT_BOTTOM_MID,0,15);//设置对齐方式
	lv_obj_set_event_cb(cb1,event_handler);//设置事件回调函数
	
	//创建开关
	sw1 = lv_sw_create(win,NULL);
	lv_obj_set_size(sw1,80,40);//设置大小
	lv_obj_align(sw1,cb1,LV_ALIGN_OUT_BOTTOM_MID,0,15);//设置对齐方式
	lv_sw_on(sw1,LV_ANIM_ON);//默认为打开状态
	lv_obj_set_event_cb(sw1,event_handler);//设置事件回调函数
	
}















