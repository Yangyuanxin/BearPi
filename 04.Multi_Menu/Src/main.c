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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
Button button ;
Menu   menu ;
Timer  timer ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/*读取按键状态*/
uint8_t read_button_pin_status(void)
{
    return HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin); ;
}

/*定时器5ms循环调用button_ticks*/
void timer1_callback()
{
    button_ticks();
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void button_callback(void *event)
{
	uint8_t button_event = get_button_event(&button) ;
	switch(button_event)
	{
		case SINGLE_CLICK:
			 Set_Event_Code(&menu, ENTER_KEY_SHORT);
			 break ;
		case LONG_RRESS_START:
			 Set_Event_Code(&menu, ENTER_KEY_LONG);
			 break ;
		default:
			 break ;
	}
}

void main_page_process(uint8_t Event_Code)
{
    switch(Event_Code)
    {
        case ENTER_KEY_SHORT:
            printf("发生单击，进入页面1\n");
            Set_Menu(&menu, LOG_PAGE);
            break ;

        case ENTER_KEY_LONG:
            printf("在页面0发生长按\n");
            break ;
    }
}

void log_page_process(uint8_t Event_Code)
{
    switch(Event_Code)
    {
        case ENTER_KEY_SHORT:
            printf("在页面1发生单击\n");
            break ;

        case ENTER_KEY_LONG:
            printf("发生长按，返回页面0\n");
            Set_Menu(&menu, MAIN_PAGE);
            break ;
    }
}

/*菜单处理*/
void Menu_Handler(struct Menu *handle)
{
	switch(handle->Current_Page)
	{
		case 0:
			main_page_process(handle->KeyEvent);
			break ;
		case 1:
			log_page_process(handle->KeyEvent);
			break ;
		default:
			break ;
	}
	/*及时将事件清除，防止重复触发*/
    Set_Event_Code(handle, NULL_KEY_EVENT);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

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
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
	/*1.初始化multi_timer*/
    timer_init(&timer, timer1_callback, 5, 5);
    timer_start(&timer);
	/*2.初始化multi_button并注册button event*/
	button_init(&button, read_button_pin_status, 0);
    button_attach(&button, SINGLE_CLICK, button_callback);
    button_attach(&button, LONG_RRESS_START, button_callback);
    button_start(&button);
    /*3.初始化菜单并注册当前菜单项及触发事件*/
    menu_init(&menu, MAIN_PAGE, NULL_KEY_EVENT);
    printf("当前页面：%d  当前事件值:%d\n", (&menu)->Current_Page,(&menu)->KeyEvent);
	
	 
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        Menu_Handler(&menu);
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

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

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

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
