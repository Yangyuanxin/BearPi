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
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "HzLib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*接收光强超时值*/
#define TIMEOUT 200
/*光强值检测阈值*/
#define LIGHT_SENSOR_THREHOLD 30


/*mode:模式选择*/
//连续高分辨率模式精度 1 lux
#define LUX_1_MODE   0x10
//连续高分辨率模式2精度 0.5 lux
#define LUX_0_5_MODE 0x11
//低分辨率模式
#define LUX_LOW_MODE 0x13

#define WRITE_ADDRESS 0x46		//0100 011  0
#define READ_ADDRESS  0x47		//0100 011  1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/*光照强度传感器结构体*/
typedef struct light_sensor
{
    /*光强值*/
    int				Lux ;
    /*multi_timer定时器句柄*/
    Timer 		timer1 ;
    /*定时器计数值*/
    uint16_t 	Timer_Count ;
    /*是否采集完成标志*/
    uint8_t   Conver_completed ;
    /*定时回调*/
    void (*timeout_cb)(void);
    void (*timeout_cb1)(void);
} light_sensor_TypeDef;
light_sensor_TypeDef lsensor ;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*定时器回调函数*/
void timer1_callback(void)
{
    ++lsensor.Timer_Count;
}


/*初始化参数*/
void Init_BH750(void)
{
    lsensor.Lux = 0 ;
    lsensor.Timer_Count = 0 ;
    lsensor.Conver_completed = 0 ;
    lsensor.timeout_cb = timer1_callback ;
}

/*读取光强*/
void ReadBH1750(uint8_t mode)
{
    float lux = 0;
    uint8_t ReadData[2] = {0};
    static uint8_t Sensor_Status = 0 ;

    /*读取光强流程*/
    switch(Sensor_Status)
    {
        /*1、发送检测光强模式的指令*/
        case 0:
            if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, WRITE_ADDRESS, (uint8_t *)&mode, 1, 0xff))
                return  ;

            Sensor_Status = 1 ;
            lsensor.Timer_Count = 0 ;
            lsensor.Conver_completed = 0 ;
            break ;

        case 1:

            /*
            	2、发送命令后延时200ms等待读取
            	定时200ms，判断是否已经到了
            	这里相当于取代了延时等待，不占用CPU
            */
            if(TIMEOUT == lsensor.Timer_Count)
            {
                lsensor.Timer_Count = 0 ;

                //3、开始读取光强，发送读光强指令
                if(HAL_OK == HAL_I2C_Master_Receive(&hi2c1, READ_ADDRESS, ReadData, 2, 0xff))
                {
                    lux = (float)((ReadData[0] << 8) | ReadData[1]);
                    lux = (double)lux / 1.2;
                    lsensor.Lux = (int)lux ;
                    /*4、转换完成*/
                    lsensor.Conver_completed = 1 ;
                    Sensor_Status = 0 ;
                }
            }

            break ;

        default:
            break;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    char display_buf[20] = {0};
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
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_SPI2_Init();
    /* USER CODE BEGIN 2 */
    /*串口初始化后加这个延时，防止后面的printf打印乱码*/
    HAL_Delay(200);
    LCD_Init();
    LCD_Clear(BLACK);//清屏为黑色
    LCD_ShowString(5, 10, 240, 32, 32, "BearPi LuxTest");//显示字符串，字体大小32*32
    printf("光强读取测试实验\n");
    Init_BH750();
    timer_init(&lsensor.timer1, lsensor.timeout_cb, 1, 1);
    timer_start(&lsensor.timer1);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        ReadBH1750(LUX_1_MODE);

        if(1 == lsensor.Conver_completed)
        {
            sprintf(display_buf, "%d%d%d%dLux", lsensor.Lux / 1000 % 100, lsensor.Lux / 100 % 10, lsensor.Lux / 10 % 10, lsensor.Lux % 10);
            LCD_ShowString(80, 50 + 24 + 32, 240, 32, 32, display_buf); //显示字符串，字体大小32*32

            if(lsensor.Lux > LIGHT_SENSOR_THREHOLD)
            {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
            }
        }

        timer_loop();

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
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;

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
