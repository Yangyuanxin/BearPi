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
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "IDReader.h"
#include "stm32l4xx_it.h"
#include "multi_timer.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//ascii转10进制
#define ASCII_TO_DEC(x)  (uint8_t)(x%(0x30))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
struct Timer Find_Id_Card_Timer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t is_read_id_card_flag = 0 ;


//1 寻卡  2 选卡 3 读取身份证信息 0 没有
__IO uint8_t Current_Cmd = 0 ;
//寻找卡回调
void Find_ID_Card_CallBack(void)
{
    switch(Current_Cmd)
    {
        //没有
        case 0:
            break ;

        //寻卡
        case 1:
            //printf("发送寻卡指令\n");
            Find_Id_Card();
            break ;

        //选卡
        case 2:
            //printf("发送选卡指令\n");
            Select_Id_Card();
            break ;

        //读取身份证信息
        case 3:
            printf("读取身份证信息\n");
            Read_Id_Card_Info();
            is_read_id_card_flag = 1 ;
            break ;
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
    int i = 0, j = 0;
    int font_info_len = 0 ;
    int pic_info_len = 0 ;
    uint8_t status = 0 ;


    ID_Card_Parse Data_Info ;
    uint8_t  Name_Buf[30] = {0};
    uint8_t  Font_Info[256] = {0};
    uint8_t  ID_Card_Number_Buf[36] = {0};

    char name_gbk[30] = {0};
    char id_card_num[30] = {0};
    char name_unicode[30] = {0};
    uint8_t Out_Put_ID_Card_Number[18] = {0};
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
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */
    printf("身份证信息读取\n");
    Reset_SAM_A();
    //开启DMA接收
    HAL_UART_Receive_DMA(&huart2, rbBuf, MAX_LEN);
    memset(rbBuf, 0, MAX_LEN);

    //启动查询定时器
    timer_init(&Find_Id_Card_Timer, Find_ID_Card_CallBack, 500, 500);
    timer_start(&Find_Id_Card_Timer);
    Current_Cmd = 1 ;
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if(Recv_Flag ==  1)
        {
            Recv_Flag = 0 ;

            if(Current_Cmd != 0)
            {
                //匹配到状态字SW1=0x00 && SW2=0x00 && SW3=0x9F，则表示已经寻找到卡了
                if(rbBuf[7] == 0x00 && rbBuf[8] == 0x00 && rbBuf[9] == 0x9F)
                    Current_Cmd = 2 ;
                //匹配到状态字SW1=0x00 && SW2=0x00 && SW3=0x90，则表示已经选择到卡了
                else if(rbBuf[7] == 0x00 && rbBuf[8] == 0x00 && rbBuf[9] == 0x90)
                    Current_Cmd = 3 ;
                else
                    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
            }

            if(is_read_id_card_flag == 1)
            {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
                //继续发送寻卡指令
                Current_Cmd = 1 ;
                is_read_id_card_flag = 0 ;
                font_info_len = rbBuf[10] << 8 | rbBuf[11] ;
                pic_info_len =  rbBuf[12] << 8 | rbBuf[13] ;

                for(i = 0 ; i < font_info_len ; i++)
                    Font_Info[i] = rbBuf[14 + i];

                memcpy(Name_Buf, Font_Info, 30);
                memcpy(ID_Card_Number_Buf, Font_Info + 122, 36);

                //打印姓名:
                for(int i = 0 ; i < 30 ; i++)
                {
                    printf("%x ", Name_Buf[i]);
                }

                printf("\n");
                sprintf(name_unicode, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
                        Name_Buf[1], Name_Buf[0], \
                        Name_Buf[3], Name_Buf[2], \
                        Name_Buf[5], Name_Buf[4], \
                        Name_Buf[7], Name_Buf[6], \
                        Name_Buf[9], Name_Buf[8], \
                        Name_Buf[11], Name_Buf[10], \
                        Name_Buf[13], Name_Buf[12], \
                        Name_Buf[15], Name_Buf[14], \
                        Name_Buf[17], Name_Buf[16], \
                        Name_Buf[19], Name_Buf[18], \
                        Name_Buf[21], Name_Buf[20], \
                        Name_Buf[23], Name_Buf[22], \
                        Name_Buf[25], Name_Buf[24], \
                        Name_Buf[27], Name_Buf[26], \
                        Name_Buf[29], Name_Buf[28]
                       );
                unistr2gbkstr(name_gbk, name_unicode);
                printf("姓名:%s\n", name_gbk);

                //打印身份证
                for(i = 0 ; i < 36 ; i++)
                {
                    if(i % 2 == 0)
                    {
                        Out_Put_ID_Card_Number[j] = ID_Card_Number_Buf[i] - 48;
                        j++;
                    }
                }

                sprintf(id_card_num, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        Out_Put_ID_Card_Number[0], Out_Put_ID_Card_Number[1],
                        Out_Put_ID_Card_Number[2], Out_Put_ID_Card_Number[3],
                        Out_Put_ID_Card_Number[4], Out_Put_ID_Card_Number[5],
                        Out_Put_ID_Card_Number[6], Out_Put_ID_Card_Number[7],
                        Out_Put_ID_Card_Number[8], Out_Put_ID_Card_Number[9],
                        Out_Put_ID_Card_Number[10], Out_Put_ID_Card_Number[11],
                        Out_Put_ID_Card_Number[12], Out_Put_ID_Card_Number[13],
                        Out_Put_ID_Card_Number[14], Out_Put_ID_Card_Number[15],
                        Out_Put_ID_Card_Number[16], Out_Put_ID_Card_Number[17]
                       );
                printf("%s的身份证号码:%s", name_gbk, id_card_num);
                j = 0;
            }


            rx_count = 0;					//清除计数
            Recv_Flag = 0;	//清除接收结束标志位
            memset(rbBuf, 0, sizeof(rbBuf));
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
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;

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
