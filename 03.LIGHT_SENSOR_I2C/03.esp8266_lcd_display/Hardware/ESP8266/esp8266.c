#include "esp8266.h"
#include "usart.h"
#include <stdarg.h>
#include "lcd.h"


wifi_ap_info esp8266_info ;
void Reset_Wifi_Para(void);
static void wifi_timeout_callback(void);
static uint8_t AT_Check_Answer(char *ack);
static void Wifi_Recv_Cmd_Process(void);


/*测试*/
void WIFI_Test(void)
{
    uint8_t ret = 0 ;
    static uint8_t err_count = 0 ;

    /*当前为发送状态*/
    if(1 == esp8266_info.tx_flag)
    {
        /*复位参数*/
        Reset_Wifi_Para();
        /*发送测试指令*/
        wifi_send_cmd(WIFI_AT_TEST);
        /*将发送状态设置为0，即为接收状态*/
        esp8266_info.tx_flag = 0 ;
        /*清空定时计数器*/
        esp8266_info.wifi_timer_count = 0 ;
        /*开启定时计数标志*/
        esp8266_info.wifi_completed_flag = 0 ;
    }
    /*当前为接收状态*/
    else
    {
        /*判断定时计数到WIFI_TEST_TIMEOUT==>1000ms了没有？*/
        if(WIFI_TEST_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            /*关闭定时计数标志*/
            esp8266_info.wifi_completed_flag = 1 ;
            /*清空定时计数器*/
            esp8266_info.wifi_timer_count = 0 ;
            /*检查DMA接收缓存中是否包含OK子串*/
            ret = AT_Check_Answer("OK");

            /*失败，错误超过3次，返回出错状态*/
            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI初始化失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                /*将状态标记为下一个指令的处理流程*/
                esp8266_info.wifi_apr_status = ITEM_WIFI_SATE ;
                printf("wifi测试成功!  回复%s\n", esp8266_info.rx_buffer);
            }
        }
    }
}

/*设置ESP8266是否回显*/
void WIFI_SET_ATE(uint8_t status)
{
    uint8_t ret = 0 ;
    static uint8_t err_count = 0 ;

    if(1 == esp8266_info.tx_flag)
    {
        Reset_Wifi_Para();
        wifi_send_cmd(WIFI_ATE_SET, status);
        esp8266_info.tx_flag = 0 ;
        esp8266_info.wifi_timer_count = 0 ;
        esp8266_info.wifi_completed_flag = 0 ;
    }
    else
    {
        if(WIFI_SET_ATE_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            esp8266_info.wifi_completed_flag = 1 ;
            esp8266_info.wifi_timer_count = 0 ;
            ret = AT_Check_Answer("OK");

            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI设置回显失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                esp8266_info.wifi_apr_status = ITEM_WIFI_SMODE ;
                printf("wifi设置回显成功!  回复%s\n", esp8266_info.rx_buffer);
            }
        }
    }
}


/*设置wifi模式*/
void WIFI_SET_MODE(uint8_t mode)
{
    uint8_t ret = 0 ;
    static uint8_t err_count = 0 ;

    if(1 == esp8266_info.tx_flag)
    {
        Reset_Wifi_Para();
        wifi_send_cmd(WIFI_AT_SET_MODE, mode);
        esp8266_info.tx_flag = 0 ;
        esp8266_info.wifi_timer_count = 0 ;
        esp8266_info.wifi_completed_flag = 0 ;
    }
    else
    {
        if(WIFI_SET_MODE_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            esp8266_info.wifi_completed_flag = 1 ;
            esp8266_info.wifi_timer_count = 0 ;
            ret = AT_Check_Answer("OK");

            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI设置AP模式失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                esp8266_info.wifi_apr_status = ITEM_WIFI_BUIAP ;
                printf("wifi设置AP模式成功!  回复%s\n", esp8266_info.rx_buffer);
            }
        }
    }
}

/*创建wifi热点*/
void WIFI_BUILD_AP_INFO(const char *ssid, const char *password, int chl, int ecn)
{
    uint8_t ret = 0 ;
    char display_buf[20] = {0};
    static uint8_t err_count = 0 ;

    if(1 == esp8266_info.tx_flag)
    {
        Reset_Wifi_Para();
        wifi_send_cmd(WIFI_AT_SAP, ssid, password, chl, ecn);
        esp8266_info.tx_flag = 0 ;
        esp8266_info.wifi_timer_count = 0 ;
        esp8266_info.wifi_completed_flag = 0 ;
    }
    else
    {
        if(WIFI_BUILD_AP_INFO_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            esp8266_info.wifi_completed_flag = 1 ;
            esp8266_info.wifi_timer_count = 0 ;
            ret = AT_Check_Answer("OK");

            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI创建热点失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                esp8266_info.wifi_apr_status = ITEM_WIFI_CMULT ;
                printf("wifi创建热点%s成功!  回复%s\n", ssid, esp8266_info.rx_buffer);
                sprintf(display_buf, "ssid:%s", ssid);
                LCD_ShowString(0, 74, 240, 24, 24, (char *)display_buf); //显示字符串，字体大小16*16
            }
        }
    }
}


/*配置多连接模式*/
void WIFI_CONFIG_MULTPLE_CONNECT(uint8_t status)
{
    uint8_t ret = 0 ;
    static uint8_t err_count = 0 ;

    if(1 == esp8266_info.tx_flag)
    {
        Reset_Wifi_Para();
        wifi_send_cmd(WIFI_AT_MULTPLE, status);
        esp8266_info.tx_flag = 0 ;
        esp8266_info.wifi_timer_count = 0 ;
        esp8266_info.wifi_completed_flag = 0 ;
    }
    else
    {
        if(WIFI_CONFIG_MULTPLE_CONNECT_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            esp8266_info.wifi_completed_flag = 1 ;
            esp8266_info.wifi_timer_count = 0 ;
            ret = AT_Check_Answer("OK");

            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI创建多连接失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                esp8266_info.wifi_apr_status = ITEM_WIFI_OSERV ;
                printf("wifi配置多连接模式成功!  回复%s\n", esp8266_info.rx_buffer);
            }
        }
    }
}

/*设置与服务器的主动断开时间*/
void WIFI_CONFIG_SERVER_TIMEOUT(int TimeOut)
{
    uint8_t ret = 0 ;
    static uint8_t err_count = 0 ;

    if(1 == esp8266_info.tx_flag)
    {
        ++err_count ;
        Reset_Wifi_Para();
        wifi_send_cmd(WIFI_SET_STO, TimeOut);
        esp8266_info.tx_flag = 0 ;
        esp8266_info.wifi_timer_count = 0 ;
        esp8266_info.wifi_completed_flag = 0 ;
    }
    else
    {
        if(WIFI_CONFIG_MULTPLE_CONNECT_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            esp8266_info.wifi_completed_flag = 1 ;
            esp8266_info.wifi_timer_count = 0 ;
            ret = AT_Check_Answer("OK");

            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI设置超时失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                esp8266_info.wifi_apr_status = ITEM_WIFI_VADDR ;
                printf("wifi设置超时成功!  回复%s\n", esp8266_info.rx_buffer);
            }
        }
    }
}


/*开启服务器模式*/
void WIFI_OPEN_SERVER_MODE(uint8_t mode, int Port)
{
    uint8_t ret = 0 ;
    static uint8_t err_count = 0 ;

    if(1 == esp8266_info.tx_flag)
    {
        ++err_count ;
        Reset_Wifi_Para();
        wifi_send_cmd(WIFI_OPEN_SMODE, mode, Port);
        esp8266_info.tx_flag = 0 ;
        esp8266_info.wifi_timer_count = 0 ;
        esp8266_info.wifi_completed_flag = 0 ;
    }
    else
    {
        if(WIFI_OPEN_SERVER_MODE_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            esp8266_info.wifi_completed_flag = 1 ;
            esp8266_info.wifi_timer_count = 0 ;
            ret = AT_Check_Answer("OK");

            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI开启服务器模式失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                esp8266_info.wifi_apr_status = ITEM_WIFI_STIMO ;
                printf("wifi开启服务器模式成功!  回复%s\n", esp8266_info.rx_buffer);
            }
        }
    }
}

/*查看地址*/
void WIFI_VIEW_IPADDR(void)
{
    uint8_t ret = 0 ;
    char ip_buf[20] = {0};
    char display_buf[20] = {0};
    static uint8_t err_count = 0 ;
    uint8_t *temp1 = NULL, *temp2 = NULL;

    if(1 == esp8266_info.tx_flag)
    {
        ++err_count ;
        Reset_Wifi_Para();
        wifi_send_cmd(WIFI_VIEW_ADDR);
        esp8266_info.tx_flag = 0 ;
        esp8266_info.wifi_timer_count = 0 ;
        esp8266_info.wifi_completed_flag = 0 ;
    }
    else
    {
        if(WIFI_VIEW_IPADDR_TIMEOUT == esp8266_info.wifi_timer_count)
        {
            esp8266_info.wifi_completed_flag = 1 ;
            esp8266_info.wifi_timer_count = 0 ;
            ret = AT_Check_Answer("OK");

            if(ret != 0)
            {
                esp8266_info.tx_flag = 1 ;
                ++err_count;

                if(err_count > 3)
                {
                    err_count = 0 ;
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;
                    printf("WIFI获取APIP地址失败\n");
                }
            }
            else
            {
                esp8266_info.tx_flag = 1 ;
                printf("wifi查询APIP地址成功!  回复%s\n", esp8266_info.rx_buffer);
                temp1 = (uint8_t*)strstr((const char*)esp8266_info.rx_buffer, "\"");
                temp2 = (uint8_t*)strstr((const char*)(temp1 + 1), "\"");

                if(temp1 == NULL || temp2 == NULL)
                    esp8266_info.wifi_apr_status = ITEM_WIFI_ERROR ;

                *temp2 = 0;
                sprintf((char *)ip_buf, "%s", temp1 + 1);
                sprintf(display_buf, "APIP:%s", ip_buf);
                LCD_ShowString(0, 98, 240, 24, 24, (char *)display_buf); //显示字符串，字体大小16*16
                memset(ip_buf, 0, 20);
                sprintf(display_buf, "PORT:%d", AP_PORT);
                LCD_ShowString(0, 122, 240, 24, 24, (char *)display_buf); //显示字符串，字体大小16*16
                HAL_UART_DMAStop(&hlpuart1);
                memset(esp8266_info.rx_buffer, 0, RX_BUFF_SIZE);
                HAL_UART_Receive_DMA(&hlpuart1, esp8266_info.rx_buffer, RX_BUFF_SIZE);
                esp8266_info.wifi_apr_status = ITEM_WIFI_GDATA ;
            }
        }
    }
}


/*ESP8266作为AP模式进行设置*/
void ESP8266_AP_Mode_Setting(void)
{
    static uint8_t error_flag = 0 ;

    switch(esp8266_info.wifi_apr_status)
    {
        case ITEM_WIFI_TEST:
            WIFI_Test();
            break ;

        case ITEM_WIFI_SATE:
            WIFI_SET_ATE(1);
            break ;

        case ITEM_WIFI_SMODE:
            WIFI_SET_MODE(2);
            break ;

        case ITEM_WIFI_BUIAP:
            WIFI_BUILD_AP_INFO(WIFI_HOT_SPOT_SSID, WIFI_HOT_SPOT_PASSWORD, 1, 4);
            break ;

        case ITEM_WIFI_CMULT:
            WIFI_CONFIG_MULTPLE_CONNECT(1);
            break ;

        case ITEM_WIFI_OSERV:
            WIFI_OPEN_SERVER_MODE(1, AP_PORT);
            break ;

        case ITEM_WIFI_STIMO:
            WIFI_CONFIG_SERVER_TIMEOUT(0);
            break ;

        case ITEM_WIFI_VADDR:
            WIFI_VIEW_IPADDR();
            break ;

        case ITEM_WIFI_GDATA:
            Wifi_Recv_Cmd_Process();
            break ;

        default:
            if(0 == error_flag)
            {
                error_flag = 1 ;

                if(ITEM_WIFI_ERROR == esp8266_info.wifi_apr_status)
                {
                    printf("WIFI出错\n");
                }
            }

            break ;
    }
}

/*wifi接收指令处理*/
static void Wifi_Recv_Cmd_Process(void)
{
    static int cmd_index = 0 ;
    char display_buf[20] = {0};
    char *cmd[] = {"LEDON", "LEDOFF", "LEDBLINK", "PLOTDISPLAY", "PLOTCLOSEDISPLAY"};

    if(strstr((char *)esp8266_info.rx_buffer, cmd[cmd_index]) != NULL)
    {
        HAL_UART_DMAStop(&hlpuart1);

        switch(cmd_index)
        {
            case 0:
                printf("接收到开灯指令\n");
                printf("接收到客户端发来的指令:%s\n", esp8266_info.rx_buffer);
                HAL_GPIO_WritePin(GPIOC, BOARD_LED_Pin, GPIO_PIN_SET);
                break ;

            case 1:
                printf("接收到关灯指令\n");
                printf("接收到客户端发来的指令:%s\n", esp8266_info.rx_buffer);
                LED_BLINK_controld(0);
                HAL_GPIO_WritePin(GPIOC, BOARD_LED_Pin, GPIO_PIN_RESET);
                break ;

            case 2:
                printf("接收到闪灯指令\n");
                printf("接收到客户端发来的指令:%s\n", esp8266_info.rx_buffer);
                LED_BLINK_controld(1);
                break ;

            case 3:
                printf("接收到显示曲线指令\n");
                printf("接收到客户端发来的指令:%s\n", esp8266_info.rx_buffer);
                plot_display_controld(1);
                break ;

            case 4:
                printf("接收到关闭显示曲线指令\n");
                printf("接收到客户端发来的指令:%s\n", esp8266_info.rx_buffer);
                plot_display_controld(0);
                break ;
        }

        LCD_Fill(0, 146, 240, 146 + 24, BLACK);
        sprintf(display_buf, "CMD:%s", cmd[cmd_index]);
        LCD_ShowString(0, 146, 240, 24, 24, (char *)display_buf); //显示字符串，字体大小16*16
        memset(esp8266_info.rx_buffer, 0, RX_BUFF_SIZE);
        HAL_UART_Receive_DMA(&hlpuart1, esp8266_info.rx_buffer, RX_BUFF_SIZE);
    }

    ++cmd_index ;

    if(5 == cmd_index)
        cmd_index = 0 ;
}




/*ESP8266作为AP模式进行初始化*/
void Init_ESP8266_AP_Mode(void)
{
    esp8266_info.tx_flag = 1 ;
    esp8266_info.wifi_apr_status = ITEM_WIFI_TEST ;
    esp8266_info.wifi_timer_count = 0 ;
    esp8266_info.wifi_completed_flag = 1 ;
    esp8266_info.wifi_timeout_cb =  wifi_timeout_callback ;
    /*开启1ms软件定时器*/
    timer_init(&esp8266_info.wifi_timer, esp8266_info.wifi_timeout_cb, 1, 1);
    timer_start(&esp8266_info.wifi_timer);
}

/*复位wifi收发参数*/
void Reset_Wifi_Para(void)
{
    HAL_UART_DMAStop(&hlpuart1);
    memset(esp8266_info.tx_buffer, 0, TX_BUFF_SIZE);
    memset(esp8266_info.rx_buffer, 0, RX_BUFF_SIZE);
    HAL_UART_Receive_DMA(&hlpuart1, esp8266_info.rx_buffer, RX_BUFF_SIZE);
}

/*wifi发送命令*/
void wifi_send_cmd(const char *format, ...)
{
    va_list args;
    uint32_t length;
    va_start(args, format);
    length = vsnprintf((char *)esp8266_info.tx_buffer, sizeof(esp8266_info.tx_buffer), (char *)format, args);
    va_end(args);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)esp8266_info.tx_buffer, length, HAL_MAX_DELAY);
}

/*0 成功  其他失败*/
static uint8_t AT_Check_Answer(char *ack)
{
    if(strstr((char *)esp8266_info.rx_buffer, ack) != NULL)
        return 0;

    return 1;
}

static void wifi_timeout_callback(void)
{
    if(0 == esp8266_info.wifi_completed_flag)
        ++esp8266_info.wifi_timer_count ;
}
