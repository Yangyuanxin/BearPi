#include "esp8266.h"
#include "usart.h"
#include <stdarg.h>
#include "lcd.h"

wifi_ap_info esp8266_info ;
static uint8_t AT_Check_Answer(uint8_t *ack);
static uint8_t AT_Cmd_Answer(uint8_t* success, uint8_t* fail, uint32_t timeout);

#define delay_ms(s) HAL_Delay(s)

//软复位
uint8_t WIFI_RST(void)
{
    uint8_t res = 0xff;
    wifi_init_printf("AT+RST\r\n");
    res = AT_Cmd_Answer((uint8_t*)"OK", NULL, 5000);
    delay_ms(1000);
    return res;
}

//测试
uint8_t WIFI_Test(void)
{
    uint8_t res = 0xff;
    wifi_init_printf("AT\r\n");
    res = AT_Cmd_Answer((uint8_t*)"OK\r\n", NULL, 200);
    return  res;
}

//设置ESP8266是否回显,返回true,表示设置成功
uint8_t WIFI_SET_ATE(uint8_t status)
{
    uint8_t res = 1 ;
    wifi_init_printf("ATE%d\r\n", status);
    res = AT_Cmd_Answer((uint8_t*)"OK\r\n", (uint8_t*)"ERROR\r\n", 200);

    if(0 != res)
        return 1 ;

    return 0 ;
}

//STA模式
uint8_t WIFI_STA_MODE(void)
{
    uint8_t res = 0xff;

    wifi_init_printf("AT+CWMODE=1\r\n");
    res = AT_Cmd_Answer((uint8_t*)"OK", (uint8_t*)"ERROR", 500);
    return res;
}

//连接路由
uint8_t WIFI_CWJAP(uint8_t* ssid, uint8_t* password)
{
    uint8_t res = 0xff;
    wifi_init_printf("AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    res = AT_Cmd_Answer((uint8_t*)"OK", NULL, 2000);
    return res;
}

//协议、 服务器IP、端口
uint8_t WIFI_CIPSTART(uint8_t* type, uint8_t* addr, uint32_t port)
{
    uint8_t res = 0xff;

    wifi_init_printf("AT+CIPSTART=\"%s\",\"%s\",%d\r\n", type, addr, port);
    res = AT_Cmd_Answer((uint8_t*)"OK", (uint8_t*)"ERROR", 2000);
    return res;
}

//设置开启透传模式
uint8_t WIFI_CIPMODE(void)
{
    uint8_t res = 0xff;

    wifi_init_printf("AT+CIPMODE=1\r\n");
    res = AT_Cmd_Answer((uint8_t*)"OK", (uint8_t*)"ERROR", 200);
    return res;
}

//开始透传
uint8_t WIFI_CIPSEND(void)
{
    uint8_t res = 0xff;

    wifi_init_printf("AT+CIPSEND\r\n");
    res = AT_Cmd_Answer((uint8_t*)"OK", (uint8_t*)"ERROR", 300);
    return res;
}

extern __IO int wifi_connect_flag ;
//WIFI初始化
void WIFI_Init(uint8_t *ssid, uint8_t *password)
{
    int ret = 0 ;
    uint8_t err_code ;
    static int wifi_status_process = 0;
    int32_t error_count = 3;
WIFI_INIT:
    WIFI_RST();

    //判断wifi在不在线
    while(error_count--)
    {
        if(WIFI_Test() == 0)
            break;
    }

    printf("wifi在线\r\n");
    error_count = 3 ;
    WIFI_SET_ATE(0);
WIFI_PROCESS_CONNECT:

    while(1)
    {
        //wifi处理状态机
        switch(wifi_status_process)
        {
            //设置模式
            case 0:
                if(0 != WIFI_STA_MODE())
                {
                    printf("设置模式出错状态\r\n");
                    goto WIFI_INIT ;
                }

                printf("1.wifi设置STA模式成功!\r\n");
                wifi_status_process = 1 ;
                break ;

            //连接路由器
            case 1:
                if(0 != WIFI_CWJAP(ssid, password))
                {
                    printf("连接路由器出错 ssid:%s password:%s\r\n", ssid, password);
                    wifi_status_process = 1 ;
                    break ;
                }

                wifi_status_process = 2 ;
                printf("2.wifi连接路由器:%s成功\r\n", ssid);
                break ;

            //连接服务器
            case 2:
                if(0 != WIFI_CIPSTART((uint8_t*)"TCP", (uint8_t*)"192.168.4.1", 8080))
                {
                    printf("连接服务器出错\r\n");
                    wifi_status_process = 1 ;
                    break ;
                }

                printf("3.wifi连接服务器:%s   端口号:%d成功\r\n", (uint8_t*)"192.168.4.1", 8080);
                wifi_status_process = 3 ;
                break ;
        }

        //如果为ok，则跳出循环
        if(3 == wifi_status_process)
            break ;
    }

    err_code = WIFI_CIPMODE();

    if(err_code != 0)
        return  ;

    printf("设置透传模式成功\n");
    err_code = WIFI_CIPSEND();

    if(err_code != 0)
        return  ;

    printf("进入透传模式成功\n");
    wifi_connect_flag = 1 ;
    LCD_ShowString(0, 100, 240, 32, 32, (char *)ssid);//显示字符串，字体大小32*32
    LCD_ShowString(0, 132, 240, 32, 32, (char *)password);//显示字符串，字体大小32*32
    LCD_ShowString(0, 164, 240, 32, 32, (char *)"192.168.4.1");//显示字符串，字体大小32*32
    LCD_ShowString(0, 196, 240, 32, 32, (char *)"8080");//显示字符串，字体大小32*32
}




void wifi_init_printf(const char *format, ...)
{
    va_list args;
    uint32_t length;

    HAL_UART_DMAStop(&hlpuart1);
    HAL_Delay(100);
    memset((char *)esp8266_info.tx_buffer, 0, TX_BUFF_SIZE);
    memset((char *)esp8266_info.rx_buffer, 0, RX_BUFF_SIZE);
    HAL_UART_Receive_DMA(&hlpuart1, esp8266_info.rx_buffer, RX_BUFF_SIZE);

    va_start(args, format);
    length = vsnprintf((char *)esp8266_info.tx_buffer, sizeof(esp8266_info.tx_buffer), (char *)format, args);
    va_end(args);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)esp8266_info.tx_buffer, length, HAL_MAX_DELAY);
}

/*0 成功  其他失败*/
static uint8_t AT_Check_Answer(uint8_t *ack)
{
    if(strstr((char *)esp8266_info.rx_buffer, (const char *)ack) != NULL)
        return 0;

    return 1;
}

//0 成功  其他失败。两个应答都为空的时候就直接返回。
static uint8_t AT_Cmd_Answer(uint8_t* success, uint8_t* fail, uint32_t timeout)
{
    uint32_t rxlen = 0, i;

    if(success == NULL && fail == NULL)
        return 0;

    while(timeout--)
    {
        if(success)
        {
            if(AT_Check_Answer(success) == 0)
            {
                return 0;
            }
        }

        if(fail)
        {
            if(AT_Check_Answer(fail) == 0)
                return 1;
        }

        if(AT_Check_Answer(success) == 0)
            return 0;

        if(AT_Check_Answer(fail) == 0)
            return 1;

        HAL_Delay(1);
    }

    return 2;
}

