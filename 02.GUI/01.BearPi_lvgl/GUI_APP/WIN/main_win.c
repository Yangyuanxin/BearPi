#include "main_win.h"
#include "gui_app.h"
#include "login_win.h"
#include "calendar_win.h"
#include "progress_win.h"
#include "chart_win.h"
#include "gauge_win.h"

//申明图片
LV_IMG_DECLARE(logo_img);
LV_IMG_DECLARE(date_img);
LV_IMG_DECLARE(chart_img);
LV_IMG_DECLARE(gauge_img);
LV_IMG_DECLARE(progress_img);
//申明字体
LV_FONT_DECLARE(my_font);//此字体里面只有正点原子四个汉字

//变量定义
#define TABLE_COL_CNT   		3   //表格的列数
#define TABLE_ROW_CNT   		3   //表格的宽度
#define SPLIT_LINE_WIDTH		200 //水平分割线的宽度
const lv_point_t SPLIT_LINE_POINTS[2] = {{0,0},{SPLIT_LINE_WIDTH,0}};
const char * const BTNM_MAP[] = {
	"    "LV_SYMBOL_AUDIO"\nAUDIO","  "LV_SYMBOL_BELL"\nBELL","\n",
	"   "LV_SYMBOL_CALL"\nCALL","  "LV_SYMBOL_GPS"\nGPS",
	""
};
const char * const BTNM_TITLE[] = {"audio btn","bell btn","call btn","gps btn"};
const char * const MBOX_MAP[] ={"Yes","No",""};
//定义表格每一行的内容
const char * const TABLE_CELL_VALUE[TABLE_ROW_CNT][TABLE_COL_CNT] = {
  {"Name","Sex","Age"},//第一行,作为标题行
  {"fish","man","25"},
  {"xjy","woman","26"}
};

lv_obj_t *  main_win;
static lv_obj_t * quit_btn;
static lv_obj_t * btnm;
static lv_obj_t * quit_mbox = NULL;
static lv_obj_t * calendar_item_btn,* progress_item_btn,* chart_item_btn,* gauge_item_btn;


//函数申明
void quit_mbox_create(lv_obj_t * parent);

//事件回调函数
static void event_handler(lv_obj_t * obj,lv_event_t event)
{
	uint16_t btn_id;
	if(obj==quit_btn)
	{
		if(event==LV_EVENT_RELEASED)
		{
			quit_mbox_create(lv_obj_get_parent(obj));//是否退出?
		}
	}else if(obj==btnm)
	{
		if(event==LV_EVENT_VALUE_CHANGED)
		{
			btn_id = *((uint16_t*)lv_event_get_data());//获取矩阵按钮中的按钮id
			toast(BTNM_TITLE[btn_id],1000);
		}
	}else if(obj==quit_mbox)
	{
		if(event==LV_EVENT_VALUE_CHANGED)
		{
			lv_obj_del(quit_mbox);//关闭退出对话框
			quit_mbox = NULL;
			btn_id = *((uint16_t*)lv_event_get_data());//获取退出对话框中的按钮id
			if(btn_id==0)//点击了Yes按钮
			{
				lv_obj_del(main_win);//删除主界面
				login_win_create(lv_scr_act());//返回到登录界面
			}
		}
	}else if(obj==calendar_item_btn)
	{
		//当列表出现滚轮时,用LV_EVENT_CLICKED来处理松手事件会比LV_EVENT_RELEASED好一些,
		//因为滑动列表时,LV_EVENT_CLICKED事件不会出现误触发,而LV_EVENT_RELEASED会误触发
		if(event==LV_EVENT_CLICKED) 
			calendar_win_create(lv_scr_act());//跳转到calendar窗体
	}else if(obj==progress_item_btn)
	{
		if(event==LV_EVENT_CLICKED)
			progress_win_create(lv_scr_act());//跳转到progress窗体
	}else if(obj==chart_item_btn)
	{
		if(event==LV_EVENT_CLICKED)
			chart_win_create(lv_scr_act());//跳转到chart窗体
	}else if(obj==gauge_item_btn)
	{
		if(event==LV_EVENT_CLICKED)
			gauge_win_create(lv_scr_act());//跳转到gauge窗体
	}
}

//创建退出确认的消息对话框
void quit_mbox_create(lv_obj_t * parent)
{
	if(quit_mbox)//不要重复创建
		return;
	quit_mbox = lv_mbox_create(parent,NULL);
	lv_obj_set_size(quit_mbox,lv_obj_get_width(parent)*0.7f,lv_obj_get_height(parent)/2);
	lv_mbox_set_text(quit_mbox,"Tip\nAre you to quit?");
	lv_mbox_add_btns(quit_mbox,(const char**)MBOX_MAP);
	lv_obj_set_drag(quit_mbox,true);//设置对话框可以被拖拽
	lv_obj_align(quit_mbox,NULL,LV_ALIGN_CENTER,0,0);
	lv_obj_set_event_cb(quit_mbox,event_handler);
}

//首页选项卡初始化
void home_tab_init(lv_obj_t * parent)
{
	//创建标题
	static lv_style_t title_style;
	lv_obj_t * title_label = lv_label_create(parent,NULL);
	lv_style_copy(&title_style,lv_obj_get_style(title_label));//复制标签之前的主题样式
	title_style.text.font = &my_font;//改变字体
	lv_label_set_style(title_label,LV_LABEL_STYLE_MAIN,&title_style);//设置新的样式
	lv_label_set_text(title_label,"\xE6\xAD\xA3\xE7\x82\xB9\xE5\x8E\x9F\xE5\xAD\x90");//这是"正点原子"的UTF-8编码
	lv_obj_align(title_label,NULL,LV_ALIGN_IN_TOP_MID,0,10);
	
	//创建列表
	lv_obj_t * list = lv_list_create(parent,NULL);
  lv_obj_set_size(list,lv_obj_get_width(parent)*0.9f,lv_obj_get_height(parent)-60);
	lv_obj_align(list,title_label,LV_ALIGN_OUT_BOTTOM_MID,0,10);
	lv_list_set_sb_mode(list,LV_SB_MODE_AUTO);
	//添加列表项1,有2行
  calendar_item_btn = lv_list_add_btn(list,&date_img,"#FFFFFF Calendar#\n#666666 This is the date#");
	lv_obj_t * item_label = lv_list_get_btn_label(calendar_item_btn);
	lv_label_set_recolor(item_label,true);
	lv_obj_set_event_cb(calendar_item_btn,event_handler);//设置事件回调函数
	//添加列表项2
  progress_item_btn = lv_list_add_btn(list,&progress_img,"Progress");
	lv_obj_set_event_cb(progress_item_btn,event_handler);//设置事件回调函数
	//添加列表项3
  chart_item_btn = lv_list_add_btn(list,&chart_img,"Chart");
	lv_obj_set_event_cb(chart_item_btn,event_handler);//设置事件回调函数
	//添加列表项4
  gauge_item_btn = lv_list_add_btn(list,&gauge_img,"Gauge");
	lv_obj_set_event_cb(gauge_item_btn,event_handler);//设置事件回调函数
	
}


//设置选项卡初始化
void setting_tab_init(lv_obj_t * parent)
{
	uint16_t parent_width = lv_obj_get_width(parent);//获取父对象的宽度
	
  //创建头像
	lv_obj_t * head_img = lv_img_create(parent,NULL);
	lv_img_set_src(head_img,&logo_img);
	lv_obj_set_pos(head_img,20,20);
	//创建用户名和id
	lv_obj_t * name_id_label = lv_label_create(parent,NULL);
	lv_label_set_recolor(name_id_label,true);
	lv_label_set_text(name_id_label,"#FFFFFF Xiong jia yu#\n#BBBBBB ID:15727652280#");
	lv_obj_align(name_id_label,head_img,LV_ALIGN_OUT_RIGHT_MID,20,0);
	//创建一根水平分割横线
	lv_obj_t * split_line = lv_line_create(parent,NULL);
	lv_obj_set_size(split_line,SPLIT_LINE_WIDTH,2);
	lv_line_set_points(split_line,SPLIT_LINE_POINTS,2);
	lv_obj_align(split_line,NULL,LV_ALIGN_IN_TOP_MID,0,100);
	//常见矩阵按钮
	btnm = lv_btnm_create(parent,NULL);
	lv_obj_set_size(btnm,parent_width*0.8f,100);
	lv_obj_align(btnm,split_line,LV_ALIGN_OUT_BOTTOM_MID,0,10);
	lv_btnm_set_map(btnm,(const char**)BTNM_MAP);
	lv_obj_set_event_cb(btnm,event_handler);
	//创建一个表格
	lv_obj_t * table = lv_table_create(parent,NULL);
	lv_table_set_col_cnt(table,TABLE_COL_CNT);//设置列数
	lv_table_set_row_cnt(table,TABLE_ROW_CNT);//设置行数
	uint8_t row,col;
	for(col=0;col<TABLE_COL_CNT;col++)
	{
		lv_table_set_col_width(table,col,parent_width*(0.8f/TABLE_COL_CNT));//设置列的宽度
		lv_table_set_cell_align(table,0,col,LV_LABEL_ALIGN_CENTER);//标题行的文本居中对齐
	}
	for(row=0;row<TABLE_ROW_CNT;row++)
	{
		for(col=0;col<TABLE_COL_CNT;col++)
			lv_table_set_cell_value(table,row,col,(const char*)TABLE_CELL_VALUE[row][col]);//设置每一个单元格的文本内容
	}
	lv_obj_align(table,btnm,LV_ALIGN_OUT_BOTTOM_MID,0,10);//设置表格的对齐方式
	
	//创建一个退出按钮
	quit_btn = lv_btn_create(parent,NULL);
	lv_obj_set_width(quit_btn,parent_width/2);
	lv_obj_align(quit_btn,table,LV_ALIGN_OUT_BOTTOM_MID,0,30);
	lv_obj_t * quit_label = lv_label_create(quit_btn,NULL);
	lv_label_set_recolor(quit_label,true);
	lv_label_set_text(quit_label,"#FF0000 Quit#");
	lv_obj_set_event_cb(quit_btn,event_handler);
	
}


//创建主界面
void main_win_create(lv_obj_t * parent)
{
	//为了减少复杂性,我们这里就直接把tabview作为main_win窗体了
	main_win = lv_tabview_create(parent,NULL);
	lv_tabview_set_btns_pos(main_win,LV_TABVIEW_BTNS_POS_BOTTOM);//选项卡按钮位于底部
	//添加tab1选项卡
  lv_obj_t * home_tab = lv_tabview_add_tab(main_win,LV_SYMBOL_HOME" Home");
  home_tab_init(home_tab);
  //添加tab1选项卡
  lv_obj_t * setting_tab = lv_tabview_add_tab(main_win,LV_SYMBOL_SETTINGS" Setting");
  setting_tab_init(setting_tab);
}










