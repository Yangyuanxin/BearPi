#ifndef __ESP8266_H
#define __ESP8266_H
#include "main.h"


/*发送数据最大长度*/
#define TX_BUFF_SIZE 50
/*接收数据最大长度*/
#define RX_BUFF_SIZE 150

/*ESP8266作为热点时的名称*/
#define WIFI_HOT_SPOT_SSID 		"BearPi_ESP8266"
/*ESP8266作为热点时的密码*/
#define WIFI_HOT_SPOT_PASSWORD	"12345678"
/*AP PORT*/
#define AP_PORT 8080

typedef struct
{
    /*wifi ap运行状态机*/
    uint8_t 	wifi_apr_status ;
    /*AT指令发送缓存*/
    uint8_t 	tx_buffer[TX_BUFF_SIZE];
    /*接收缓存*/
    uint8_t   rx_buffer[RX_BUFF_SIZE];
    /*发送标志*/
    uint8_t  	tx_flag ;
    /*multi_timer定时器句柄*/
    Timer 		wifi_timer ;
    /*定时器计数值*/
    uint16_t 	wifi_timer_count ;
    /*wifi完成标志*/
    uint8_t 	wifi_completed_flag ;
    /*定时回调*/
    void (*wifi_timeout_cb)(void);
} wifi_ap_info ;



/*测试WIFI*/
#define WIFI_AT_TEST     	"AT\r\n"
/*设置或关闭回显*/
#define WIFI_ATE_SET	    "ATE%d\r\n"
/*设置WIFI模式*/
#define WIFI_AT_SET_MODE 	"AT+CWMODE=%d\r\n"
/*创建WIFI热点*/
#define WIFI_AT_SAP			"AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n"
/*配置多连接模式*/
#define WIFI_AT_MULTPLE		"AT+CIPMUX=%d\r\n"
/*开启服务器模式*/
#define WIFI_OPEN_SMODE		"AT+CIPSERVER=%d,%d\r\n"
/*设置与服务器的主动断开时间*/
#define WIFI_SET_STO 		"AT+CIPSTO=%d\r\n"
/*查看WIFI作为服务器时的地址*/
#define WIFI_VIEW_ADDR		"AT+CIFSR\r\n"


/*每个状态机执行的超时查询时间*/
#define WIFI_TEST_TIMEOUT 	       			1000
#define WIFI_SET_ATE_TIMEOUT       			200
#define WIFI_SET_MODE_TIMEOUT 	   			200
#define WIFI_BUILD_AP_INFO_TIMEOUT 			4000
#define WIFI_CONFIG_MULTPLE_CONNECT_TIMEOUT 200
#define WIFI_OPEN_SERVER_MODE_TIMEOUT		1000
#define WIFI_VIEW_IPADDR_TIMEOUT			1000

/*每个状态机对应的序号*/
enum
{
    ITEM_WIFI_TEST = 0,
    ITEM_WIFI_SATE,
    ITEM_WIFI_SMODE,
    ITEM_WIFI_BUIAP,
    ITEM_WIFI_CMULT,
    ITEM_WIFI_OSERV,
    ITEM_WIFI_STIMO,
    ITEM_WIFI_VADDR,
    ITEM_WIFI_GDATA,
    ITEM_WIFI_ERROR = 99
};

/*ESP8266作为AP模式进行初始化*/
void Init_ESP8266_AP_Mode(void);
/*Wifi作为服务器时的服务*/
void ESP8266_AP_Mode_Setting(void);
/*wifi发送命令*/
void wifi_send_cmd(const char *format, ...);

#endif //__ESP8266_h

