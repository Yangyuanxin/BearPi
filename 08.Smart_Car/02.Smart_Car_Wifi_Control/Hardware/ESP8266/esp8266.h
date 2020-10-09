#ifndef __ESP8266_H
#define __ESP8266_H
#include "main.h"


/*发送数据最大长度*/
#define TX_BUFF_SIZE 50
/*接收数据最大长度*/
#define RX_BUFF_SIZE 1024

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


void wifi_init_printf(const char *format, ...);
//WIFI初始化
void WIFI_Init(uint8_t *ssid, uint8_t *password);

#endif //__ESP8266_h

