/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "sdmmc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "iniparser.h"
#include "multi_button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Button button1, button2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch, FILE *stream)
{
    /* 堵塞判断串口是否发送完成 */
    while((USART1->ISR & 0X40) == 0);

    /* 串口发送完成，将该字符发送 */
    USART1->TDR = (uint8_t) ch;

    return ch;
}

/*文件名*/
#define SETTING_PARA "0:Para.ini"

/*默认系统配置信息*/
char *System_Config_Info =
    "[Setting]\n"
    "led_flag=1;\n"									/*LED标志*/
    "plot_flag=0;\n"									/*曲线开关*/
    "WIFI_NAME=BearPi_ESP8266;\n"					/*WIFI热点*/
    "WIFI_PASSWORD=12345678;\n"						/*WIFI密码*/
    ;

typedef struct
{
    int led_flag ;
    int plot_flag ;
    char wifi_name[32] ;
    char wifi_password[32];
} info ;

info ini_info ;
dictionary  *Config_ini = NULL;

/*系统配置文件全局变量*/
uint8_t retUSER_SYS_CONFIG ;
FATFS USERFatFS_SYS_CONFIG ;
FIL USER_SYS_CONFIG_File ;

/*挂载SD卡*/
int Mount_SD(void)
{
    /*挂载SD卡*/
    retSD = f_mount(&SDFatFS, SDPath, 1);

    if(FR_OK != retSD)
        return -1 ;

    return 0 ;
}

/*创建一个默认的配置文件*/
void Create_Default_InI_File(void)
{
    retUSER_SYS_CONFIG = f_open(&USER_SYS_CONFIG_File, SETTING_PARA, FA_OPEN_ALWAYS | FA_WRITE);

    if(FR_OK != retUSER_SYS_CONFIG)
    {
        fprintf(stderr, "iniparser: cannot open %s\n", SETTING_PARA);
        return ;
    }

    f_printf(&USER_SYS_CONFIG_File, System_Config_Info);
    f_close(&USER_SYS_CONFIG_File);
}

/*加载INI文件*/
int Load_Confg_INI_Process(void)
{
    /*加载INI文件*/
    Config_ini = iniparser_load(SETTING_PARA);

    if(NULL == Config_ini)
    {
        printf("加载出错\n");
        Create_Default_InI_File();
        Config_ini = iniparser_load(SETTING_PARA);

        if(NULL == Config_ini)
        {
            printf("创建默认INI文件后继续加载出错\n");
            return  -1;
        }
    }

    printf("加载INI文件成功\n");
    return 0 ;
}

/*保存参数*/
int INI_Para_Save_Process(void)
{
    /*write config.ini parse*/
    retUSER_SYS_CONFIG = f_open(&USER_SYS_CONFIG_File, SETTING_PARA, FA_OPEN_EXISTING | FA_WRITE);

    if(FR_OK != retUSER_SYS_CONFIG)
    {
        printf("iniparser: cannot open %s\n", SETTING_PARA);
        return -1;
    }

    printf("参数设置保存成功\n");
    iniparser_dump_ini(Config_ini, &USER_SYS_CONFIG_File);
    f_close(&USER_SYS_CONFIG_File);
    iniparser_freedict(Config_ini);
    return 0 ;
}



uint8_t read_button1()
{
    return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) ;
}

uint8_t read_button2()
{
    return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) ;
}


void button1_callback(void *ptr)
{
    static uint8_t index = 0 ;
    char *wifi_name_para[] = {"Yangyuanxin", "Bruce_Yang", "BearPi", "mculover666"};

    if(index == 4)
        index = 0 ;

    printf("\r\n按下KEY1，改变并保存WIFI_NAME参数！\n");
    Load_Confg_INI_Process();
    iniparser_set(Config_ini, "Setting:WIFI_NAME", wifi_name_para[index]);
    INI_Para_Save_Process();
	printf("设置wifi_name:%s成功\n",wifi_name_para[index]);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    ++index ;
}

void button2_callback(void *ptr)
{
	char *wifi_name = NULL ;
    printf("\r\n按下KEY2，读取WIFI_NAME参数！\n");
    Load_Confg_INI_Process();
    wifi_name = iniparser_getstring(Config_ini, 	  "Setting:WIFI_NAME", 	   "not found");
    INI_Para_Save_Process();
    memset(ini_info.wifi_name, 0, strlen(ini_info.wifi_name));
    memcpy(ini_info.wifi_name, wifi_name, strlen(wifi_name));
    printf("wifi_name:%s\n", ini_info.wifi_name);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    int ret = -1 ;
    char *wifi_name = NULL ;
    char *wifi_password = NULL ;
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_SDMMC1_SD_Init();
    MX_USART1_UART_Init();
    MX_FATFS_Init();
    /* USER CODE BEGIN 2 */
    button_init(&button1, read_button1, 0);
    button_init(&button2, read_button2, 0);
    button_attach(&button1, SINGLE_CLICK, button1_callback);
    button_attach(&button2, SINGLE_CLICK, button2_callback);
	button_start(&button1);
	button_start(&button2);
	
	
    ret = Mount_SD();

    if(ret != 0)
    {
        printf("SD Card mount ERROR\r\n");
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        return -1 ;
    }

    printf("SD卡挂载成功!\n");
    ret = Load_Confg_INI_Process();

    if(ret != 0)
    {
        printf("读取INI文件失败!\r\n");
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        return -2 ;
    }

    /*加载系统参数*/
    ini_info.led_flag = iniparser_getint(Config_ini,  "Setting:led_flag",  -1);
    ini_info.plot_flag = iniparser_getint(Config_ini, "Setting:plot_flag", -1);
    wifi_name = iniparser_getstring(Config_ini, 	  "Setting:WIFI_NAME", 	   "not found");
    wifi_password = iniparser_getstring(Config_ini,   "Setting:WIFI_PASSWORD", "not found");
    memcpy(ini_info.wifi_name, wifi_name, strlen(wifi_name));
    memcpy(ini_info.wifi_password, wifi_password, strlen(wifi_password));
    /*改变参数*/
    ret = iniparser_set(Config_ini, "Setting:led_flag", "0");
    if(ret != 0)
    {
        printf("改变参数失败!\n");
        return -3 ;
    }

    printf("改变参数成功\n");
    ret = INI_Para_Save_Process();

    if(ret != 0)
    {
        printf("写入保存INI文件失败!\r\n");
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        return -4 ;
    }

    printf("写入保存INI文件成功!\r\n");
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
		button_ticks();
		HAL_Delay(5);
    }

    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_SDMMC1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
