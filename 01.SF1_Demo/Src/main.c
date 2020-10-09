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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "HzLib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct Sensor
{
    /*烟感值*/
    int Smoke_Value;
    /*报警灯状态*/
    uint8_t Alarm_Led_Status : 1;
    /*报警蜂鸣器状态*/
    uint8_t Alarm_Buzzer_Status : 1;
    /*是否打印曲线标志*/
    uint8_t Display_Plot_Flag : 1 ;

    void (*led_control)(struct Sensor *sensor_handle, uint8_t status);
    void (*buzzer_control)(struct Sensor *sensor_handle, uint8_t status);
    void (*plot_control)(struct Sensor *sensor_handle, uint8_t status);

    int (*get_smoke_value)(struct Sensor *sensor_handle);
    int (*get_led_status)(struct Sensor *sensor_handle);
    int (*get_buzzer_status)(struct Sensor *sensor_handle);
    int (*get_plot_status)(struct Sensor *sensor_handle);
} Sensor ;
Sensor *MQ2_Sensor = NULL;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern __IO int Timer_Count  ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void alarm_led_control(Sensor *sensor_handle, uint8_t status)
{
    sensor_handle->Alarm_Led_Status = status ;

    if(sensor_handle->Alarm_Led_Status)
        HAL_GPIO_WritePin(SF1_LED_GPIO_Port, SF1_LED_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SF1_LED_GPIO_Port, SF1_LED_Pin, GPIO_PIN_RESET);
}

void alarm_buzzer_control(Sensor *sensor_handle, uint8_t status)
{
    sensor_handle->Alarm_Buzzer_Status = status ;

    if(sensor_handle->Alarm_Buzzer_Status)
        HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
    else
        HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
}

void plot_display_control(Sensor *sensor_handle, uint8_t status)
{
    sensor_handle->Display_Plot_Flag = status ;
}

int get_led_status(Sensor *sensor_handle)
{
    return sensor_handle->Alarm_Led_Status ;
}

int get_buzzer_status(Sensor *sensor_handle)
{
    return sensor_handle->Alarm_Buzzer_Status ;
}

int get_plot_status(Sensor *sensor_handle)
{
    return sensor_handle->Display_Plot_Flag ;
}

int get_smoke_value(Sensor *sensor_handle)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 50);
    sensor_handle->Smoke_Value = HAL_ADC_GetValue(&hadc1);
    return sensor_handle->Smoke_Value ;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*提供访问传感器的接口*/
Sensor mq2_sensor_interface =
{
    /*设置接口*/
    .led_control = alarm_led_control,
    .buzzer_control = alarm_buzzer_control,
    .plot_control = plot_display_control,
    /*获取接口*/
    .get_led_status = get_led_status,
    .get_buzzer_status = get_buzzer_status,
    .get_plot_status = get_plot_status,
    .get_smoke_value = get_smoke_value
};



/*注册传感器*/
int Sensor_Register(Sensor *sensor_handle)
{
    if(NULL == MQ2_Sensor)
    {
        MQ2_Sensor = sensor_handle ;

        if(NULL == MQ2_Sensor)
            return -1 ;
        else
        {
            MQ2_Sensor->led_control(MQ2_Sensor, 0);
            MQ2_Sensor->buzzer_control(MQ2_Sensor, 0);
            MQ2_Sensor->plot_control(MQ2_Sensor, 0);
        }
    }

    return 0 ;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    int smoke_value = 0 ;
    int serial_number = 0 ;
    char procol_buf[20] = {0};
    char display_buf[20] = {0};
    static uint8_t buzzer_flag = 0;
    static uint8_t led_status = 0 ;
    static uint8_t plot_status = 0 ;
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
    MX_ADC1_Init();
    MX_SPI2_Init();
    MX_USART1_UART_Init();
    MX_TIM16_Init();
    /* USER CODE BEGIN 2 */
    Sensor_Register(&mq2_sensor_interface);
    printf("led_status:%d\n", mq2_sensor_interface.get_led_status(&mq2_sensor_interface));
    printf("buzzer_status:%d\n", mq2_sensor_interface.get_buzzer_status(&mq2_sensor_interface));
    printf("plot_status:%d\n", mq2_sensor_interface.get_plot_status(&mq2_sensor_interface));
    LCD_Init();
    LCD_Clear(BLACK);//清屏为黑色
    LCD_ShowString(5, 10, 240, 32, 32, "BearPi MQ-2");//显示字符串，字体大小32*32
    Timer_Count = 0 ;

    /*设置曲线显示*/
    mq2_sensor_interface.plot_control(&mq2_sensor_interface, 1);
    plot_status = mq2_sensor_interface.get_plot_status(&mq2_sensor_interface);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        //printf("烟感值:%d\n",mq2_sensor_interface.get_smoke_value(&mq2_sensor_interface));
        /*判断是否100ms到了，该计数由Systick提供，1ms中断一次，100次则为100ms*/
        if(100 == Timer_Count)
        {
            Timer_Count = 0 ;
            /*流水号自增*/
            ++serial_number ;

            if(serial_number > 65535)
                serial_number = 0 ;

            smoke_value = mq2_sensor_interface.get_smoke_value(&mq2_sensor_interface) ;

            /*当烟感值大于600ppm，则报警*/
            if(smoke_value > 600)
            {
                /*这里的buzzer_flag只执行一次，避免重复执行*/
                if(0 == buzzer_flag)
                {
                    buzzer_flag = 1 ;
                    mq2_sensor_interface.buzzer_control(&mq2_sensor_interface, 1);
                }

                led_status = !led_status ;
                mq2_sensor_interface.led_control(&mq2_sensor_interface, led_status);
            }
            else
            {
                /*这里的buzzer_flag只执行一次，避免重复执行*/
                if(1 == buzzer_flag)
                {
                    buzzer_flag = 0 ;
                    mq2_sensor_interface.buzzer_control(&mq2_sensor_interface, 0);
                    led_status = 0 ;
                    mq2_sensor_interface.led_control(&mq2_sensor_interface, led_status);
                }
            }

            /*在LCD上显示烟感值*/
            sprintf(display_buf, "%d%d%d%dppm", smoke_value / 1000 % 100, smoke_value / 100 % 10, smoke_value / 10 % 10, smoke_value % 10);
            LCD_ShowString(80, 50 + 24 + 32, 240, 32, 32, display_buf); //显示字符串，字体大小32*32
            /*拼接发送协议*/
            sprintf(procol_buf, "S1 %d%d%d%d%d %d%d%d%d%d",
                    serial_number / 10000,
                    serial_number / 1000 % 100 % 10,
                    serial_number / 100 % 10,
                    serial_number / 10 % 10,
                    serial_number % 10,
                    smoke_value / 10000,
                    smoke_value / 1000 % 100 % 10,
                    smoke_value / 100 % 10,
                    smoke_value / 10 % 10,
                    smoke_value % 10
                   );

            /*如果曲线标志为1，则打印曲线*/
            if(1 == plot_status)
                printf("%s \r\n", procol_buf);
        }
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

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_ADC;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;

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
